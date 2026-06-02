// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/S1Player.h"
#include "S1Enums.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Weapon/S1Weapon.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Player/S1PlayerController.h"
#include "Player/S1PlayerState.h"
#include "S1GameplayTags.h"

AS1Player::AS1Player()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;  
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
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


	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));	
	SpringArm->TargetArmLength = 800.f;							
	SpringArm->bUsePawnControlRotation = true;					

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
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

	AbilitySystemComponent->AddCharacterAbilities(CharacterAbilitiesTag);
}

void AS1Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitSystem();

	if (nullptr == WeaponClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	EquippedWeapon = GetWorld()->SpawnActor<AS1Weapon>(WeaponClass, SpawnParams);
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	EquippedWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));
}

void AS1Player::InitSystem()
{
	Super::InitSystem();

	if (AS1PlayerState* PS = GetPlayerState<AS1PlayerState>())
	{
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

