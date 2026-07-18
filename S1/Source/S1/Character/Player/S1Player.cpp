// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/S1Player.h"
#include "S1Enums.h"
#include "Camera/S1PlayerCameraComponent.h"
#include "Component/S1LockOnComponent.h"
#include "Component/S1EquipComponent.h"
#include "Component/S1InteractComponent.h"
#include "Component/S1PlayerReactBridgeComponent.h"
#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "Animation/Weapon/S1WeaponAnimLayer.h"
#include "Animation/S1AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Item/S1HealItem.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "NiagaraComponent.h"
#include "System/S1SoundManager.h"
#include "Data/S1WeaponData.h"
#include "System/S1AssetManager.h"
#include "Weapon/S1Weapon.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Player/S1PlayerController.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "S1LogChannels.h"
#include "Components/InputComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/World.h"

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

	// Interact (버튼 등 상호작용 오브젝트 탐지)
	InteractComponent = CreateDefaultSubobject<US1InteractComponent>(TEXT("InteractComponent"));

	// 피격 리액션 브릿지 (GE 적용 감지 → GA_Hit 트리거)
	ReactBridgeComponent = CreateDefaultSubobject<US1PlayerReactBridgeComponent>(TEXT("ReactBridgeComponent"));

	// AtkCollision 가상 충돌체 — AS1Weapon::AttackBox와 동일한 콜리전 프로파일(기본 NoCollision), GA_Attack이 히트 윈도우에서만 켬
	VirtualSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("VirtualSphereCollision"));
	VirtualSphereCollision->SetupAttachment(BodyMesh);
	VirtualSphereCollision->SetCollisionProfileName(VirtualCollisionProfileName);
	VirtualSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VirtualSphereCollision->SetGenerateOverlapEvents(true);

	VirtualBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("VirtualBoxCollision"));
	VirtualBoxCollision->SetupAttachment(BodyMesh);
	VirtualBoxCollision->SetCollisionProfileName(VirtualCollisionProfileName);
	VirtualBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VirtualBoxCollision->SetGenerateOverlapEvents(true);

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

	// 무기 무관 공용 어빌리티(GA_Hit 등) — EquipWeapon의 WeaponAbilitiesTag 그룹과 별개 그룹이라
	// 무기 교체(RemoveCharacterAbilities(CurrentWeaponAbilitiesTag))에 영향받지 않고 계속 유지됨
	if (IsValid(AbilitySystemComponent) && DefaultAbilitiesTag.IsValid())
	{
		AbilitySystemComponent->AddCharacterAbilities(DefaultAbilitiesTag);
	}

	// 피격 리액션 트리거를 US1PlayerSet::PostGameplayEffectExecute로 이전 — ReactBridgeComponent는 당분간 비활성(삭제는 안 함).
	// 다시 쓰려면 아래 주석 해제 (단, PlayerSet 쪽 HandleGameplayEvent 호출과 동시에 켜면 GA_Hit 중복 트리거됨).
	// if (IsValid(ReactBridgeComponent))
	// {
	// 	ReactBridgeComponent->BindAbilitySystem();
	// }

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

	// ItemTag 없으면(맨손) WeaponData의 기본 무기 엔트리로 대체 — 무기 유무와 무관하게 동일한 조회 경로 사용
	const FGameplayTag ResolvedItemTag = ItemTag.IsValid() ? ItemTag : DefaultWeaponTag;

	TSubclassOf<AS1Weapon> ResolvedWeaponClass        = nullptr;
	TSubclassOf<AS1Weapon> ResolvedOffhandWeaponClass = nullptr;
	FGameplayTag ResolvedWeaponAbilitiesTag           = FGameplayTag::EmptyTag;
	TSubclassOf<US1WeaponAnimLayer> ResolvedAnimLayerClass = nullptr;

	US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData);
	if (IsValid(WeaponData))
	{
		if (const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(ResolvedItemTag))
		{
			ResolvedWeaponClass        = Entry->WeaponClass;
			ResolvedOffhandWeaponClass = Entry->OffhandWeaponClass;
			ResolvedWeaponAbilitiesTag = Entry->WeaponAbilitiesTag;
			ResolvedAnimLayerClass     = Entry->GetAnimLayerClass(Gender);

			if (nullptr == ResolvedAnimLayerClass)
			{
				LOG_WARNING(TEXT("EquipWeapon: [%s] has no AnimLayerClass."), *ResolvedItemTag.ToString());
			}
		}
		else
		{
			LOG_WARNING(TEXT("EquipWeapon: WeaponData has no entry for [%s]."), *ResolvedItemTag.ToString());
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
	// ItemTag(원본)이 아닌 ResolvedItemTag(맨손 폴백 반영값)를 써야 함 — 안 그러면 맨손(Default) 최초 장착 시
	// Empty→Empty로 값이 안 바뀌어 리플리케이션이 발생하지 않고 클라에서 OnRep이 아예 호출되지 않음
	EquippedItemTag = ResolvedItemTag;
}

void AS1Player::ServerRequestEquip_Implementation(const FGameplayTag& ItemTag)
{
	EquipWeapon(ItemTag);
}

void AS1Player::SpawnHealItem(TSubclassOf<AS1HealItem> HealItemClass, ES1AttackHand Hand, const FTransform& AttachOffset, const FRotator& RotationRate, float DissolveDuration)
{
	if (nullptr == HealItemClass)
	{
		return;
	}

	if (IsValid(CurrentHealItem))
	{
		CurrentHealItem->Destroy();
		CurrentHealItem = nullptr;
	}

	const FName TargetSocket = (Hand == ES1AttackHand::Offhand) ? OffhandSocketName : WeaponSocketName;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	CurrentHealItem = GetWorld()->SpawnActor<AS1HealItem>(HealItemClass, SpawnParams);
	if (false == IsValid(CurrentHealItem))
	{
		return;
	}

	// 위치만 소켓에 스냅 — 회전은 현재 상대 회전(ItemMesh는 SetAbsolute로 이미 소켓 회전과 디커플링됨)을 그대로 유지
	static const FAttachmentTransformRules HealItemAttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);
	CurrentHealItem->AttachToComponent(GetMesh(), HealItemAttachRules, TargetSocket);
	CurrentHealItem->SetActorRelativeTransform(AttachOffset);
	CurrentHealItem->SetRotationRate(RotationRate);
	CurrentHealItem->PlaySpawnDissolve(DissolveDuration);
}

void AS1Player::DespawnHealItem(float DissolveDuration)
{
	if (IsValid(CurrentHealItem))
	{
		CurrentHealItem->PlayDespawnDissolve(DissolveDuration);
		CurrentHealItem = nullptr;
	}
}

void AS1Player::MulticastResetHealItemPresentation_Implementation()
{
	// 정상 종료 시 이미 노티파이가 정리했다면 여기서는 전부 아무 동작 안 함(멱등) — 인터럽트로 못 끝났을 때만 실제로 복구
	const float RecoveryDissolveDuration = 0.15f;

	if (IsValid(CurrentHealItem))
	{
		CurrentHealItem->PlayDespawnDissolve(RecoveryDissolveDuration);
		CurrentHealItem = nullptr;
	}

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->PlayDissolve(true, RecoveryDissolveDuration);
	}

	if (IsValid(EquippedOffhandWeapon))
	{
		EquippedOffhandWeapon->PlayDissolve(true, RecoveryDissolveDuration);
	}
}

void AS1Player::MulticastPlayLevelUpPresentation_Implementation()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (AS1NiagaraEffect* EffectCDO = S1EffectSpawnLibrary::FindNiagaraEffectCDO(S1AssetTags::Asset_Effect, S1EffectTags::Effect_Player_LevelUp))
		{
			UNiagaraComponent* Component = nullptr;
			EffectCDO->PlayEffectAttached(GetMesh(), TEXT("VFX_b_C_Base"), FTransform::Identity, &Component);

			if (nullptr != Component)
			{
				// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
				Component->SetAbsolute(false, false, true);
			}
		}
	}

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		const FGameplayTag SoundTag = (Gender == EPlayerGender::Female) ? S1SoundTags::Sound_Player_Female_LevelUp : S1SoundTags::Sound_Player_Male_LevelUp;
		SoundManager->PlaySoundAtLocationByTag(SoundTag, GetActorLocation());
	}
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
	const FGameplayTag ResolvedItemTag = EquippedItemTag.IsValid() ? EquippedItemTag : DefaultWeaponTag;

	TSubclassOf<US1WeaponAnimLayer> ResolvedAnimLayerClass = nullptr;

	if (US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData))
	{
		if (const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(ResolvedItemTag))
		{
			ResolvedAnimLayerClass = Entry->GetAnimLayerClass(Gender);

			if (nullptr == ResolvedAnimLayerClass)
			{
				LOG_WARNING(TEXT("OnRep_EquippedItemTag: [%s] has no AnimLayerClass."), *ResolvedItemTag.ToString());
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

void AS1Player::TryInteract()
{
	if (false == IsValid(InteractComponent))
	{
		return;
	}

	if (AS1PuzzleButton_Gimmick* Target = InteractComponent->GetNearestInteractable())
	{
		ServerInteract(Target);
	}
}

void AS1Player::ServerInteract_Implementation(AS1PuzzleButton_Gimmick* Target)
{
	if (false == IsValid(Target))
	{
		return;
	}

	// 클라가 보고한 대상을 서버가 거리로 재검증(치트/랙 방지).
	const float DistSq = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	if (DistSq > FMath::Square(InteractValidationRadius))
	{
		return;
	}

	Target->PressButton();
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

	// 지상 — State.Air 하위 전부 + State.Used.Air 하위 전부 제거(둘은 서로 다른 트리라 따로 매칭 필요)
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	for (const FGameplayTag& Tag : OwnedTags)
	{
		if (Tag.MatchesTag(AirStateTag) || (UsedAirStateTag.IsValid() && Tag.MatchesTag(UsedAirStateTag)))
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

void AS1Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if !UE_BUILD_SHIPPING
	if (IsValid(PlayerInputComponent))
	{
		PlayerInputComponent->BindKey(EKeys::NumPadSeven, IE_Pressed, this, &ThisClass::DebugTriggerHit_Weak);
		PlayerInputComponent->BindKey(EKeys::NumPadEight, IE_Pressed, this, &ThisClass::DebugTriggerHit_Strong);
		PlayerInputComponent->BindKey(EKeys::NumPadNine,  IE_Pressed, this, &ThisClass::DebugTriggerHit_ToAir);
		PlayerInputComponent->BindKey(EKeys::Zero,        IE_Pressed, this, &ThisClass::DebugSetUltimateGaugeMax);
		PlayerInputComponent->BindKey(EKeys::Nine,        IE_Pressed, this, &ThisClass::DebugDamageSelf50Percent);
		PlayerInputComponent->BindKey(EKeys::Eight,       IE_Pressed, this, &ThisClass::DebugForceLevelUp);
	}
#endif
}

void AS1Player::DebugTriggerHit_Weak()
{
	ServerDebugTriggerHit(S1HitType::HitType_Weak);
}

void AS1Player::DebugTriggerHit_Strong()
{
	ServerDebugTriggerHit(S1HitType::HitType_Strong);
}

void AS1Player::DebugTriggerHit_ToAir()
{
	ServerDebugTriggerHit(S1HitType::HitType_ToAir);
}

void AS1Player::ServerDebugTriggerHit_Implementation(FGameplayTag HitTypeTag)
{
#if !UE_BUILD_SHIPPING
	if (false == IsValid(AbilitySystemComponent) || false == HitTypeTag.IsValid())
	{
		return;
	}

	// 공격원 없이 테스트 — 월드 원점(WorldSettings 위치, 보통 0,0,0)을 공격원 위치로 취급
	FGameplayEventData Payload;
	Payload.EventTag   = HitTypeTag;
	Payload.Instigator = GetWorld() ? GetWorld()->GetWorldSettings() : nullptr;
	Payload.Target     = this;

	LOG(TEXT("[DebugHit] %s 강제 트리거 (원점 기준)"), *HitTypeTag.ToString());
	AbilitySystemComponent->HandleGameplayEvent(HitTypeTag, &Payload);
#endif
}

void AS1Player::DebugSetUltimateGaugeMax()
{
	ServerDebugSetUltimateGaugeMax();
}

void AS1Player::ServerDebugSetUltimateGaugeMax_Implementation()
{
#if !UE_BUILD_SHIPPING
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	US1PlayerSet* PlayerSet = const_cast<US1PlayerSet*>(Cast<US1PlayerSet>(AbilitySystemComponent->GetAttributeSet(US1PlayerSet::StaticClass())));
	if (nullptr == PlayerSet)
	{
		return;
	}

	PlayerSet->AddUltimateGauge(PlayerSet->GetMaxUltimateGauge());

	LOG(TEXT("[DebugUltimateGauge] 게이지 Max 설정 — Current: %.0f / %.0f"),
		PlayerSet->GetCurrentUltimateGauge(), PlayerSet->GetMaxUltimateGauge());
#endif
}

void AS1Player::DebugDamageSelf50Percent()
{
	ServerDebugDamageSelf50Percent();
}

void AS1Player::ServerDebugDamageSelf50Percent_Implementation()
{
#if !UE_BUILD_SHIPPING
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (false == IsValid(AttributeSet))
	{
		return;
	}

	const float NewHealth = FMath::Max(0.f, AttributeSet->GetHealth() - AttributeSet->GetMaxHealth() * 0.5f);
	AttributeSet->SetHealth(NewHealth);

	LOG(TEXT("[DebugDamage] HP 50%% 감소 — Current: %.0f / %.0f"), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
#endif
}

void AS1Player::DebugForceLevelUp()
{
	ServerDebugForceLevelUp();
}

void AS1Player::ServerDebugForceLevelUp_Implementation()
{
#if !UE_BUILD_SHIPPING
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	US1PlayerSet* PlayerSet = const_cast<US1PlayerSet*>(Cast<US1PlayerSet>(AbilitySystemComponent->GetAttributeSet(US1PlayerSet::StaticClass())));
	if (nullptr == PlayerSet)
	{
		return;
	}

	PlayerSet->LevelUp();

	LOG(TEXT("[DebugLevelUp] 강제 레벨업 — Level: %.0f"), PlayerSet->GetLevel());
#endif
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
	else if (AbilitySystemComponent->IsAbilityPredicted(AbilityTag))
	{
		// LocalPredicted — 클라에서 로컬 GA에도 직접 전달 (Activate와 동일한 흐름)
		AbilitySystemComponent->ReleaseAbility(AbilityTag);
		ServerReleaseAbility(AbilityTag);
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

	// 시전자만 하이라이트되도록 스텐실 전환 (멀티에서 다른 플레이어도 Player=1이라 구분 안 됨)
	SetUltimateStencilActive(true);

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
	SetUltimateStencilActive(false);

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

void AS1Player::SetUltimateStencilActive(bool bActive)
{
	const uint8 StencilValue = bActive
		? (uint8)ES1StencilLayer::UltimateSpotlight
		: (uint8)ES1StencilLayer::Player;

	GetMesh()->SetCustomDepthStencilValue(StencilValue);
	HairMesh->SetCustomDepthStencilValue(StencilValue);
	FaceMesh->SetCustomDepthStencilValue(StencilValue);

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(StencilValue);
	}

	if (IsValid(EquippedOffhandWeapon))
	{
		EquippedOffhandWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(StencilValue);
	}
}

