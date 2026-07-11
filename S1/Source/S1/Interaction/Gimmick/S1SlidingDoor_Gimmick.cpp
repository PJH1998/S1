// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1SlidingDoor_Gimmick.h"

#include "Components/StaticMeshComponent.h"

AS1SlidingDoor_Gimmick::AS1SlidingDoor_Gimmick()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DoorRoot);

	LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));
	LeftDoorMesh->SetupAttachment(DoorRoot);

	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));
	RightDoorMesh->SetupAttachment(DoorRoot);
}

void AS1SlidingDoor_Gimmick::BeginPlay()
{
	Super::BeginPlay();

	LeftClosedRelativeLocation = LeftDoorMesh->GetRelativeLocation();
	RightClosedRelativeLocation = RightDoorMesh->GetRelativeLocation();

	// 현재 복제 상태에 맞춰 초기 위치 스냅(late-join 클라 포함).
	StartTransition(false == IsUsable(), /*bInstant=*/true);
}

void AS1SlidingDoor_Gimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (false == bIsMoving)
	{
		return;
	}

	MoveElapsedTime += DeltaTime;
	const float Alpha = (OpenDuration > 0.f) ? FMath::Clamp(MoveElapsedTime / OpenDuration, 0.f, 1.f) : 1.f;

	LeftDoorMesh->SetRelativeLocation(FMath::Lerp(LeftMoveStartLocation, LeftMoveTargetLocation, Alpha));
	RightDoorMesh->SetRelativeLocation(FMath::Lerp(RightMoveStartLocation, RightMoveTargetLocation, Alpha));

	if (Alpha >= 1.f)
	{
		bIsMoving = false;
	}
}

void AS1SlidingDoor_Gimmick::OnRep_IsActivated()
{
	// 전환 지점(서버 ActivateGimmick/ResetGimmick·클라 OnRep 공통).
	StartTransition(false == IsUsable(), /*bInstant=*/false);
}

void AS1SlidingDoor_Gimmick::StartTransition(bool bOpen, bool bInstant)
{
	LeftMoveStartLocation = LeftDoorMesh->GetRelativeLocation();
	RightMoveStartLocation = RightDoorMesh->GetRelativeLocation();

	LeftMoveTargetLocation = bOpen ? (LeftClosedRelativeLocation + LeftOpenOffset) : LeftClosedRelativeLocation;
	RightMoveTargetLocation = bOpen ? (RightClosedRelativeLocation + RightOpenOffset) : RightClosedRelativeLocation;

	if (bInstant)
	{
		LeftDoorMesh->SetRelativeLocation(LeftMoveTargetLocation);
		RightDoorMesh->SetRelativeLocation(RightMoveTargetLocation);
		bIsMoving = false;
		return;
	}

	MoveElapsedTime = 0.f;
	bIsMoving = true;
}
