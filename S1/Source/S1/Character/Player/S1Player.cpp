// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/S1Player.h"
#include "S1Enums.h"
#include "Camera/S1PlayerCameraComponent.h"
#include "Component/S1LockOnComponent.h"
#include "Component/S1EquipComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
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

	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->AddCharacterAbilities(DefaultAbilitiesTag);
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
	// ItemTag에 따라 엔트리 결정
	TSubclassOf<AS1Weapon> ResolvedWeaponClass             = DefaultWeaponClass;
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
				ResolvedAnimLayerClass     = Entry->AnimLayerClass;
				ResolvedWeaponAbilitiesTag = Entry->WeaponAbilitiesTag;
			}
		}
	}

	ResolvedWeaponAbilitiesTag = AppendGenderSuffix(ResolvedWeaponAbilitiesTag);

	// 무기 BP 교체 — 아이템 변경 시 항상 교체 (외형/히트박스 다를 수 있음)
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	if (nullptr != ResolvedWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		EquippedWeapon = GetWorld()->SpawnActor<AS1Weapon>(ResolvedWeaponClass, SpawnParams);
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		EquippedWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));
	}

	// WeaponType이 달라진 경우에만 AnimLayer 교체
	const ES1WeaponType NewWeaponType = IsValid(EquippedWeapon) ? EquippedWeapon->GetWeaponType() : ES1WeaponType::None;

	if (NewWeaponType != CurrentWeaponType)
	{
		if (nullptr != ResolvedAnimLayerClass)
		{
			GetMesh()->LinkAnimClassLayers(ResolvedAnimLayerClass);

			if (US1AnimInstance* AnimInst = Cast<US1AnimInstance>(GetMesh()->GetAnimInstance()))
			{
				AnimInst->RequestInertialization(WeaponLayerBlendTime);
			}
		}

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
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(ActionStateTag))
	{
		return;
	}

	if (IsValid(AbilitySystemComponent) && JumpEventTag.IsValid())
	{
		FGameplayEventData EventData;
		AbilitySystemComponent->HandleGameplayEvent(JumpEventTag, &EventData);
	}

	Super::Jump();

	if (IsValid(AbilitySystemComponent) && AirStateTag.IsValid() && !GetCharacterMovement()->IsFalling())
	{
		AbilitySystemComponent->AddLooseGameplayTag(AirStateTag);
	}
}

void AS1Player::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (AirStateTag.IsValid())
	{
		FGameplayTagContainer OwnedTags;
		AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

		for (const FGameplayTag& Tag : OwnedTags)
		{
			if (Tag.MatchesTag(AirStateTag))
			{
				AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
			}
		}
	}

	if (LandedEventTag.IsValid())
	{
		FGameplayEventData EventData;
		AbilitySystemComponent->HandleGameplayEvent(LandedEventTag, &EventData);
	}
}

void AS1Player::SetSprinting(bool bInSprint)
{
	bSprint = bInSprint;
	GetCharacterMovement()->MaxWalkSpeed = bSprint ? SprintSpeed : WalkSpeed;
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

	AbilitySystemComponent->ActivateAbility(AbilityTag);
}

void AS1Player::ReleaseAbility(const FGameplayTag& AbilityTag)
{
	if (false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->ReleaseAbility(AbilityTag);
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

