// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"

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
}

void AS1Player::InitAbilitySystem()
{
	Super::InitAbilitySystem();
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

