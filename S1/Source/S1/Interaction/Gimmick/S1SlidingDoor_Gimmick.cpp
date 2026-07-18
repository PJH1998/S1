// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1SlidingDoor_Gimmick.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "System/S1SequenceManager.h"
#include "System/S1SoundManager.h"

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

		// 열림 완료음 — 각 머신 로컬 재생(데디서버 제외는 SoundManager가 처리). 닫힘 완료는 제외.
		if (bIsActivated && OpenCompleteSoundTag.IsValid())
		{
			if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
			{
				SoundManager->PlaySoundAtLocationByTag(OpenCompleteSoundTag, GetActorLocation());
			}
		}

		// 열림 이동이 막 끝난 시점에만(닫힘 이동 완료는 제외) 리빌 시퀀스를 끊고 카메라/입력을 복구한다.
		// 로컬 플레이어가 있는 머신(스탠드얼론/리슨서버 호스트/각 클라)에서만 — 데디케이티드 서버는 제외.
		// 주의: GetFirstPlayerController()는 데디케이티드 서버에서도 null이 아니라 "첫 번째로 연결된
		// 클라의 PlayerController"를 그대로 반환한다(IsValid()만으론 안 걸러짐) — 반드시 IsLocalController()로 확인해야 함.
		if (bIsActivated && RevealSequenceTag.IsValid())
		{
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (::IsValid(PC) && PC->IsLocalController())
			{
				if (US1SequenceManager* SequenceManager = GetWorld()->GetSubsystem<US1SequenceManager>())
				{
					SequenceManager->StopSequenceByTag(RevealSequenceTag);
				}

				// 입력 바인딩은 PlayerController에 있음(AS1PlayerController::SetupInputComponent) — Pawn이 아니라 PC를 Enable해야 함.
				if (APawn* Pawn = PC->GetPawn())
				{
					PC->SetViewTargetWithBlend(Pawn, RevealExitBlendTime, VTBlend_EaseInOut, 2.f);
				}

				PC->EnableInput(PC);
			}
		}
	}
}

void AS1SlidingDoor_Gimmick::OnRep_IsActivated()
{
	Super::OnRep_IsActivated();   // OnActivatedCosmetic 브로드캐스트

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
