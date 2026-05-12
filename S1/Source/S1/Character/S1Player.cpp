// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1PlayerSet.h"
#include "Player/S1PlayerController.h"
#include "Player/S1PlayerState.h"

AS1Player::AS1Player()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

}

void AS1Player::BeginPlay()
{
	Super::BeginPlay();
}

void AS1Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitSystem();
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

void AS1Player::ActivateAbility(const FGameplayTag& AbilityTag)
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	AbilitySystemComponent->ActivateAbility(AbilityTag);
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

