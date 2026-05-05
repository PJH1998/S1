// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AS1Player::AS1Player()
{
	// SpringArm — 캐릭터 머리 위 높이에서 뒤로 뻗음
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));	// 머리 높이로 피벗 올리기
	SpringArm->TargetArmLength = 500.f;							// 발까지 보이는 거리
	SpringArm->bUsePawnControlRotation = true;					// 마우스로 카메라 회전

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;					// SpringArm이 회전 담당
}

void AS1Player::BeginPlay()
{
	Super::BeginPlay();
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

