// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/S1Player.h"
#include "S1Enums.h"
#include "Camera/S1PlayerCameraComponent.h"
#include "Component/S1LockOnComponent.h"
#include "Component/S1EquipComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "Animation/Weapon/S1WeaponAnimLayer.h"
#include "Animation/S1AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/S1WeaponData.h"
#include "System/S1AssetManager.h"
#include "Weapon/S1Weapon.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Player/S1PlayerController.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"
#include "Net/UnrealNetwork.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "MovieSceneTimeController.h"
#include "MovieScene.h"
#include "MovieSceneSpawnable.h"
#include "MovieScenePossessable.h"
#include "MovieSceneSequenceID.h"
#include "CineCameraActor.h"
#include "GameFramework/PlayerController.h"

AS1Player::AS1Player()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;  
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	//BodyMesh
	USkeletalMeshComponent* BodyMesh = GetMesh();
	BodyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	// 데디 서버에서도 본 갱신 — 몽타주 노티파이/소켓(무기 히트 판정) 정확도 보장
	BodyMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	//HairMesh
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Hair");
	HairMesh->SetupAttachment(BodyMesh);
	HairMesh->SetLeaderPoseComponent(BodyMesh);
	
	//FaceMesh
	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Face");
	FaceMesh->SetupAttachment(BodyMesh);
	FaceMesh->SetLeaderPoseComponent(BodyMesh);

	// LockOn
	LockOnComponent = CreateDefaultSubobject<US1LockOnComponent>(TEXT("LockOnComponent"));

	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));	
	SpringArm->TargetArmLength = 800.f;							
	SpringArm->bUsePawnControlRotation = true;					

	// Camera
	Camera = CreateDefaultSubobject<US1PlayerCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Custom Depth: 머티리얼 오클루전 페이드 감지용
	BodyMesh->SetRenderCustomDepth(true);
	BodyMesh->SetCustomDepthStencilValue((uint8)ES1StencilLayer::Player);
	HairMesh->SetRenderCustomDepth(true);
	HairMesh->SetCustomDepthStencilValue((uint8)ES1StencilLayer::Player);
	FaceMesh->SetRenderCustomDepth(true);
	FaceMesh->SetCustomDepthStencilValue((uint8)ES1StencilLayer::Player);
}

void AS1Player::BeginPlay()
{
	Super::BeginPlay();

	// 어빌리티 부여는 서버 권위 (GiveAbility → 클라 복제)
	if (false == HasAuthority() || false == IsValid(AbilitySystemComponent))
	{
		return;
	}
}

void AS1Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라: PlayerState 복제 도착 시 ASC ActorInfo 초기화 (서버는 PossessedBy에서 처리)
	if (AS1PlayerState* PS = GetPlayerState<AS1PlayerState>())
	{
		AbilitySystemComponent = Cast<US1AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		if (IsValid(AbilitySystemComponent))
		{
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}

		AttributeSet = PS->GetS1PlayerSet();
	}
}

void AS1Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitSystem();

	FGameplayTag InitialTag = FGameplayTag::EmptyTag;

	if (AS1PlayerState* PS = GetPlayerState<AS1PlayerState>())
	{
		if (US1EquipComponent* EquipComp = PS->GetEquipComponent())
		{
			EquipComp->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);

			InitialTag = EquipComp->GetEquippedItemTag(S1EquipSlotTags::Equip_Type_Weapon);
		}
	}

	// EquipComponent 없으면 기본 무기로 초기화
	EquipWeapon(InitialTag);

	AbilitySystemComponent->AddCharacterAbilities(DefaultAbilitiesTag);
}

void AS1Player::OnItemEquipped(FGameplayTag ItemTag)
{
	EquipWeapon(ItemTag);
}

FGameplayTag AS1Player::AppendGenderSuffix(const FGameplayTag& BaseTag) const
{
	if (false == BaseTag.IsValid())
	{
		return BaseTag;
	}

	const TCHAR* Suffix = (Gender == EPlayerGender::Female) ? TEXT(".Female") : TEXT(".Male");
	return FGameplayTag::RequestGameplayTag(FName(*(BaseTag.GetTagName().ToString() + Suffix)));
}

void AS1Player::EquipWeapon(const FGameplayTag& ItemTag)
{
	// 스폰/부착/GA 부여는 서버 권위 — 클라 호출은 서버로 전달
	if (false == HasAuthority())
	{
		ServerRequestEquip(ItemTag);
		return;
	}

	// ItemTag에 따라 엔트리 결정
	const bool bFemale = (Gender == EPlayerGender::Female);

	TSubclassOf<AS1Weapon> ResolvedWeaponClass             = DefaultWeaponClass;
	TSubclassOf<AS1Weapon> ResolvedOffhandWeaponClass      = nullptr;
	TSubclassOf<US1WeaponAnimLayer> ResolvedAnimLayerClass = DefaultAnimLayerClass;
	FGameplayTag ResolvedWeaponAbilitiesTag                 = DefaultWeaponAbilitiesTag;

	if (ItemTag.IsValid())
	{
		US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData);
		if (IsValid(WeaponData))
		{
			if (const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(ItemTag))
			{
				ResolvedWeaponClass        = Entry->WeaponClass;
				ResolvedOffhandWeaponClass = Entry->OffhandWeaponClass;
				ResolvedWeaponAbilitiesTag = Entry->WeaponAbilitiesTag;

				TSubclassOf<US1WeaponAnimLayer> EntryLayerClass = bFemale ? Entry->FemaleAnimLayerClass : Entry->MaleAnimLayerClass;
				if (nullptr != EntryLayerClass)
				{
					ResolvedAnimLayerClass = EntryLayerClass;
				}
				else
				{
					LOG_WARNING(TEXT("EquipWeapon: [%s] has no %s AnimLayerClass — falling back to default."),
						*ItemTag.ToString(), bFemale ? TEXT("Female") : TEXT("Male"));
				}
			}
		}
	}

	ResolvedWeaponAbilitiesTag = AppendGenderSuffix(ResolvedWeaponAbilitiesTag);

	// 무기 BP 교체 — 아이템 변경 시 항상 교체 (외형/히트박스 다를 수 있음). 복제 액터라 클라에 자동 반영
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	if (IsValid(EquippedOffhandWeapon))
	{
		EquippedOffhandWeapon->Destroy();
		EquippedOffhandWeapon = nullptr;
	}

	if (nullptr != ResolvedWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		EquippedWeapon = GetWorld()->SpawnActor<AS1Weapon>(ResolvedWeaponClass, SpawnParams);
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		EquippedWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));
	}

	if (nullptr != ResolvedOffhandWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		EquippedOffhandWeapon = GetWorld()->SpawnActor<AS1Weapon>(ResolvedOffhandWeaponClass, SpawnParams);
		EquippedOffhandWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OffhandSocketName);
		EquippedOffhandWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));
	}

	// WeaponType이 달라진 경우에만 AnimLayer 교체 (서버 자신의 히트 판정 포즈용)
	const ES1WeaponType NewWeaponType = IsValid(EquippedWeapon) ? EquippedWeapon->GetWeaponType() : ES1WeaponType::None;

	if (NewWeaponType != CurrentWeaponType)
	{
		LinkWeaponAnimLayer(ResolvedAnimLayerClass);
		CurrentWeaponType = NewWeaponType;
	}

	// 무기 GA 교체 — 이전 무기 GA 회수 후 새 무기 GA 부여
	if (IsValid(AbilitySystemComponent))
	{
		if (CurrentWeaponAbilitiesTag.IsValid())
		{
			AbilitySystemComponent->RemoveCharacterAbilities(CurrentWeaponAbilitiesTag);
		}

		if (ResolvedWeaponAbilitiesTag.IsValid())
		{
			AbilitySystemComponent->AddCharacterAbilities(ResolvedWeaponAbilitiesTag);
		}

		CurrentWeaponAbilitiesTag = ResolvedWeaponAbilitiesTag;
	}

	// 복제 트리거 — 원격 클라가 OnRep에서 AnimLayer 링크
	EquippedItemTag = ItemTag;
}

void AS1Player::ServerRequestEquip_Implementation(const FGameplayTag& ItemTag)
{
	EquipWeapon(ItemTag);
}

void AS1Player::LinkWeaponAnimLayer(TSubclassOf<US1WeaponAnimLayer> AnimLayerClass)
{
	if (nullptr == AnimLayerClass)
	{
		return;
	}

	GetMesh()->LinkAnimClassLayers(AnimLayerClass);

	if (US1AnimInstance* AnimInst = Cast<US1AnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInst->RequestInertialization(WeaponLayerBlendTime);
	}
}

void AS1Player::OnRep_EquippedItemTag()
{
	// 원격 클라 비주얼 — 장착 태그로 AnimLayer 클래스 재해석 후 링크
	const bool bFemale = (Gender == EPlayerGender::Female);
	TSubclassOf<US1WeaponAnimLayer> ResolvedAnimLayerClass = DefaultAnimLayerClass;

	if (EquippedItemTag.IsValid())
	{
		if (US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData))
		{
			if (const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(EquippedItemTag))
			{
				TSubclassOf<US1WeaponAnimLayer> EntryLayerClass = bFemale ? Entry->FemaleAnimLayerClass : Entry->MaleAnimLayerClass;
				if (nullptr != EntryLayerClass)
				{
					ResolvedAnimLayerClass = EntryLayerClass;
				}
				else
				{
					LOG_WARNING(TEXT("OnRep_EquippedItemTag: [%s] has no %s AnimLayerClass — falling back to default."),
						*EquippedItemTag.ToString(), bFemale ? TEXT("Female") : TEXT("Male"));
				}
			}
		}
	}

	LinkWeaponAnimLayer(ResolvedAnimLayerClass);
}

void AS1Player::InitSystem()
{
	Super::InitSystem();

	if (AS1PlayerState* PS = GetPlayerState<AS1PlayerState>())
	{
		PS->InitPlayerSetFromTable(S1AssetTags::Asset_DataTable, S1DataTableTags::DataTable_PlayerData);

		AbilitySystemComponent = Cast<US1AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		AttributeSet = PS->GetS1PlayerSet();
	}

	if (AS1PlayerController* PC = Cast<AS1PlayerController>(GetController()))
	{
		PlayerController = PC;
	}
}

void AS1Player::Jump()
{
	// 액션 중 점프 차단 — LocalPredicted라 소유 클라 로컬 GA가 State.Action 보유
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(ActionStateTag))
	{
		return;
	}

	// 이동은 CMC가 클라 예측 + 서버 복제
	Super::Jump();

	// GAS 측(점프 이벤트 + 공중 태그) — 소유 클라 로컬(예측) + 서버 양쪽에 적용
	HandleJumpGAS();

	if (false == HasAuthority())
	{
		ServerNotifyJump();
	}
}

void AS1Player::ServerNotifyJump_Implementation()
{
	HandleJumpGAS();
}

void AS1Player::HandleJumpGAS()
{
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	// 점프 이벤트만 발송 — 공중 태그는 OnMovementModeChanged가 관리 (CMC 이동모드 기준)
	if (JumpEventTag.IsValid())
	{
		FGameplayEventData EventData;
		AbilitySystemComponent->HandleGameplayEvent(JumpEventTag, &EventData);
	}
}

void AS1Player::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// 공중 태그 제거는 OnMovementModeChanged(MOVE_Walking 진입)가 처리 — 여기선 착지 이벤트만
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (LandedEventTag.IsValid())
	{
		FGameplayEventData EventData;
		AbilitySystemComponent->HandleGameplayEvent(LandedEventTag, &EventData);
	}
}

void AS1Player::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (false == IsValid(AbilitySystemComponent) || false == AirStateTag.IsValid())
	{
		return;
	}

	// CMC 이동모드는 소유 클라(예측)·서버에서 동일하게 전이 → State.Air 동기 유지
	ApplyAirState(GetCharacterMovement()->MovementMode == MOVE_Falling);
}

void AS1Player::ApplyAirState(bool bInAir)
{
	if (false == IsValid(AbilitySystemComponent) || false == AirStateTag.IsValid())
	{
		return;
	}

	if (bInAir)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(AirStateTag, 1);
		return;
	}

	// 지상 — State.Air 및 모든 하위(State.Air.Used.WeakAttack 등)까지 제거
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	for (const FGameplayTag& Tag : OwnedTags)
	{
		if (Tag.MatchesTag(AirStateTag))
		{
			AbilitySystemComponent->SetLooseGameplayTagCount(Tag, 0);
		}
	}
}

void AS1Player::SetSprinting(bool bInSprint)
{
	// 소유 클라 로컬 적용(예측) + 서버 전달
	bSprint = bInSprint;
	GetCharacterMovement()->MaxWalkSpeed = bSprint ? SprintSpeed : WalkSpeed;

	if (false == HasAuthority())
	{
		ServerSetSprinting(bInSprint);
	}
}

void AS1Player::ServerSetSprinting_Implementation(bool bInSprint)
{
	bSprint = bInSprint;
	GetCharacterMovement()->MaxWalkSpeed = bSprint ? SprintSpeed : WalkSpeed;
}

void AS1Player::SetLastInputDirection(const FVector& InDirection)
{
	LastInputDirection = InDirection;

	// 서버 GA가 후방 입력 차단에 사용 — 방향이 의미있게 바뀐 경우에만 전송
	if (false == HasAuthority())
	{
		const bool bZeroChanged = (InDirection.IsNearlyZero() != LastSentInputDirection.IsNearlyZero());
		if (bZeroChanged || FVector::DotProduct(InDirection, LastSentInputDirection) < 0.98f)
		{
			LastSentInputDirection = InDirection;
			ServerSetLastInputDirection(InDirection);
		}
	}
}

void AS1Player::ServerSetLastInputDirection_Implementation(const FVector& InDirection)
{
	LastInputDirection = InDirection;
}

void AS1Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1Player, EquippedWeapon);
	DOREPLIFETIME(AS1Player, EquippedOffhandWeapon);
	DOREPLIFETIME(AS1Player, EquippedItemTag);
	DOREPLIFETIME_CONDITION(AS1Player, bSprint, COND_SkipOwner);
}

bool AS1Player::GetSprinting()
{
	return bSprint;
}

void AS1Player::ActivateAbility(const FGameplayTag& AbilityTag)
{
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (HasAuthority())
	{
		// 호스트는 서버 상태가 곧 자기 상태라 보정 불필요
		AbilitySystemComponent->ActivateAbility(AbilityTag);
	}
	else if (AbilitySystemComponent->IsAbilityPredicted(AbilityTag))
	{
		// LocalPredicted — 클라에서 직접 활성화 → GAS가 예측키 붙여 서버에 동기화 (루트모션 AbilityTask 예측 가능)
		AbilitySystemComponent->ActivateAbility(AbilityTag);

		// 콤보 어드밴스(재활성화)였다면 GAS 자동 RPC가 없으므로, 클라가 생성한 예측키를 서버에 직접 전달
		// → 서버가 같은 키로 재활성화를 미러 (콤보 타별 루트모션이 공유 키로 네트워킹됨)
		if (US1PlayerAbilitySystemComponent* PlayerASC = Cast<US1PlayerAbilitySystemComponent>(AbilitySystemComponent))
		{
			const FPredictionKey ReactivationKey = PlayerASC->ConsumePendingReactivationKey();
			if (ReactivationKey.IsValidKey())
			{
				ServerReactivateAbility(AbilityTag, ReactivationKey);
			}
		}
	}
	else
	{
		// ServerOnly — 입력을 서버로 전달, 클라의 정확한 공중 상태도 함께 (서버 이동상태 랙 보정)
		const bool bInAir = AirStateTag.IsValid() && AbilitySystemComponent->HasMatchingGameplayTag(AirStateTag);
		ServerActivateAbility(AbilityTag, bInAir);
	}
}

void AS1Player::ServerActivateAbility_Implementation(const FGameplayTag& AbilityTag, bool bInAir)
{
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	// 서버의 State.Air(+하위)를 클라 보고값으로 보정 — autonomous proxy의 서버 측 이동상태 랙으로
	// 지상/공중 공격이 잘못 선택되는 것 방지 (이후 OnMovementModeChanged가 다시 동기)
	ApplyAirState(bInAir);

	AbilitySystemComponent->ActivateAbility(AbilityTag);
}

void AS1Player::ServerReactivateAbility_Implementation(const FGameplayTag& AbilityTag, FPredictionKey Key)
{
	US1PlayerAbilitySystemComponent* PlayerASC = Cast<US1PlayerAbilitySystemComponent>(AbilitySystemComponent);
	if (false == IsValid(PlayerASC))
	{
		return;
	}

	// 클라가 생성한 예측키로 스코프 → 서버 재활성화의 루트모션이 클라와 같은 키로 생성되어 동기화됨
	FScopedPredictionWindow ScopedPrediction(PlayerASC, Key);
	PlayerASC->HandleServerInputPressed(AbilityTag);
}

void AS1Player::ReleaseAbility(const FGameplayTag& AbilityTag)
{
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (HasAuthority())
	{
		AbilitySystemComponent->ReleaseAbility(AbilityTag);
	}
	else
	{
		ServerReleaseAbility(AbilityTag);
	}
}

void AS1Player::ServerReleaseAbility_Implementation(const FGameplayTag& AbilityTag)
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->ReleaseAbility(AbilityTag);
	}
}

void AS1Player::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(ActionStateTag))
	{
		return;
	}

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ── Ultimate 컷씬 (시전 클라 로컬 프레젠테이션) ──────────────────────────

void AS1Player::ClientPlayUltimateCutscene_Implementation(ULevelSequence* Sequence, FName InPlayerBindingTag, float InEnterBlendTime, float InExitBlendTime)
{
	UltiSequence          = Sequence;
	UltiPlayerBindingTag  = InPlayerBindingTag;
	UltiEnterBlendTime    = InEnterBlendTime;
	UltiExitBlendTime     = InExitBlendTime;
	bUltiCameraBlendRetried = false;
	bUltiExitBlendStarted   = false;

	StartUltimateCutscene();
}

void AS1Player::ClientStopUltimateCutscene_Implementation()
{
	// 취소: 블렌드 없이 즉시 게임플레이 카메라 복귀 (카메라 파괴 전)
	if (APlayerController* PC = GetController<APlayerController>())
	{
		PC->SetViewTargetWithBlend(this, 0.f);
	}
	UltiCleanup();
}

void AS1Player::StartUltimateCutscene()
{
	if (false == ::IsValid(UltiSequence))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		return;
	}

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;

	ALevelSequenceActor* OutActor = nullptr;
	UltiSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, UltiSequence, Settings, OutActor);
	UltiSequenceActor = OutActor;
	if (false == ::IsValid(UltiSequencePlayer) || false == ::IsValid(UltiSequenceActor))
	{
		return;
	}

	UltiSequencePlayer->SetTimeController(MakeShared<FMovieSceneTimeController_PlatformClock>());

	// 플레이어 앵커만 주입 (반드시 Play 이전) — 카메라는 시퀀스가 Spawnable로 직접 스폰
	UltiSequenceActor->SetBindingByTag(UltiPlayerBindingTag, { this });

	UDefaultLevelSequenceInstanceData* InstanceData = NewObject<UDefaultLevelSequenceInstanceData>(UltiSequenceActor);
	InstanceData->TransformOrigin = GetActorTransform();
	UltiSequenceActor->DefaultInstanceData = InstanceData;
	UltiSequenceActor->bOverrideInstanceData = true;

	UltiSequencePlayer->OnFinished.AddDynamic(this, &ThisClass::OnUltiCutsceneFinished);
	UltiSequencePlayer->Play();

	// 스폰된 카메라를 찾아 진입 블렌드
	UltiTryBlendToCineCamera();

	// 시퀀스 종료 (ExitBlendTime + Buffer) 전에 복귀 블렌드 시작 — Spawnable 시네캠이 살아있는 동안
	const FQualifiedFrameTime TotalDuration = UltiSequencePlayer->GetDuration();
	const float SequenceSec     = static_cast<float>(TotalDuration.Time.AsDecimal() / TotalDuration.Rate.AsDecimal());
	const float Buffer          = 0.1f;
	const float BlendStartDelay = FMath::Max(0.f, SequenceSec - UltiExitBlendTime - Buffer);

	World->GetTimerManager().SetTimer(
		UltiExitStartTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() { UltiStartExitBlend(); }),
		FMath::Max(0.01f, BlendStartDelay),
		false);
}

void AS1Player::UltiTryBlendToCineCamera()
{
	APlayerController* PC = GetController<APlayerController>();
	if (false == ::IsValid(PC))
	{
		return;
	}

	UltiCineCamera = UltiFindSequenceCineCamera();
	if (false == ::IsValid(UltiCineCamera))
	{
		// 스폰 평가가 다음 틱일 수 있음 — 1회만 재시도
		if (false == bUltiCameraBlendRetried)
		{
			bUltiCameraBlendRetried = true;
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateWeakLambda(this, [this]() { UltiTryBlendToCineCamera(); }));
			}
		}
		return;
	}

	PC->SetViewTargetWithBlend(UltiCineCamera, UltiEnterBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f);
}

ACineCameraActor* AS1Player::UltiFindSequenceCineCamera() const
{
	if (false == ::IsValid(UltiSequencePlayer))
	{
		return nullptr;
	}

	UMovieSceneSequence* Sequence = UltiSequencePlayer->GetSequence();
	if (nullptr == Sequence)
	{
		return nullptr;
	}

	UMovieScene* MovieScene = Sequence->GetMovieScene();
	if (nullptr == MovieScene)
	{
		return nullptr;
	}

	const int32 SpawnableCount = MovieScene->GetSpawnableCount();
	for (int32 Index = 0; Index < SpawnableCount; ++Index)
	{
		const FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(Index);
		for (const TWeakObjectPtr<>& WeakObject : UltiSequencePlayer->FindBoundObjects(Spawnable.GetGuid(), MovieSceneSequenceID::Root))
		{
			if (ACineCameraActor* FoundCamera = Cast<ACineCameraActor>(WeakObject.Get()))
			{
				return FoundCamera;
			}
		}
	}

	const int32 PossessableCount = MovieScene->GetPossessableCount();
	for (int32 Index = 0; Index < PossessableCount; ++Index)
	{
		const FMovieScenePossessable& Possessable = MovieScene->GetPossessable(Index);
		for (const TWeakObjectPtr<>& WeakObject : UltiSequencePlayer->FindBoundObjects(Possessable.GetGuid(), MovieSceneSequenceID::Root))
		{
			if (ACineCameraActor* FoundCamera = Cast<ACineCameraActor>(WeakObject.Get()))
			{
				return FoundCamera;
			}
		}
	}

	return nullptr;
}

void AS1Player::UltiStartExitBlend()
{
	if (bUltiExitBlendStarted)
	{
		return;
	}
	bUltiExitBlendStarted = true;

	APlayerController* PC = GetController<APlayerController>();

	// 시네캠이 살아있는 동안 복귀 블렌드 시작 → 디스폰 시점엔 이미 폰 도달
	if (::IsValid(PC))
	{
		PC->SetViewTargetWithBlend(this, UltiExitBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			UltiCleanupTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]() { UltiCleanup(); }),
			FMath::Max(0.01f, UltiExitBlendTime),
			false);
	}
}

void AS1Player::OnUltiCutsceneFinished()
{
	// 폴백: 타이머가 못 돈 경우 지금 복귀 블렌드 시작
	UltiStartExitBlend();
}

void AS1Player::UltiCleanup()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(UltiExitStartTimer);
		World->GetTimerManager().ClearTimer(UltiCleanupTimer);
	}

	if (::IsValid(UltiSequencePlayer))
	{
		UltiSequencePlayer->OnFinished.RemoveAll(this);
		if (UltiSequencePlayer->IsPlaying())
		{
			UltiSequencePlayer->Stop();
		}
	}

	// Spawnable 카메라는 Keep State라 종료 후에도 남음 — 직접 파괴
	if (::IsValid(UltiCineCamera))
	{
		UltiCineCamera->Destroy();
		UltiCineCamera = nullptr;
	}

	if (::IsValid(UltiSequenceActor))
	{
		UltiSequenceActor->Destroy();
		UltiSequenceActor = nullptr;
	}

	UltiSequencePlayer = nullptr;
	UltiSequence = nullptr;
}

