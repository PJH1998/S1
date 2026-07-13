// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1TogglePillar_Gimmick.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AS1TogglePillar_Gimmick::AS1TogglePillar_Gimmick()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(false);

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	SetRootComponent(PillarMesh);

	// bIsActivated 기본값(false)이 곧 "최고 높이(배치된 그대로)" — 베이스 클래스 기본값 그대로 사용.
}

void AS1TogglePillar_Gimmick::SetInitialRaised(bool bRaised)
{
	// bIsActivated == true는 "최저 높이(lowered)"를 뜻하므로 반대로 대입한다.
	bIsActivated = (false == bRaised);
}

void AS1TogglePillar_Gimmick::SetSpawnBaseLocation(const FVector& InLocation)
{
	InitialLocation = InLocation;
}

void AS1TogglePillar_Gimmick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1TogglePillar_Gimmick, InitialLocation);
}

void AS1TogglePillar_Gimmick::BeginPlay()
{
	Super::BeginPlay();

	// InitialLocation은 코디네이터가 스폰 전 SetSpawnBaseLocation으로 주입하고 Replicated로
	// 클라까지 동기화된 값을 그대로 쓴다. GetActorLocation()으로 다시 캡처하지 않는다 —
	// bReplicateMovement가 꺼져 있어 클라의 액터 트랜스폼은 "서버가 스폰 직후 자신의 BeginPlay에서
	// 이미 위치를 옮긴 뒤" 시점 기준으로 동기화될 수 있고, 그 값을 기준으로 다시 캡처하면
	// 이미 내려간 위치를 기준 삼아 중복으로 더 내려가 버리는 버그가 생긴다.

	// 현재 복제 상태에 맞춰 초기 위치 스냅(late-join 클라 포함).
	StartTransition(false == IsUsable(), /*bInstant=*/true);
}

void AS1TogglePillar_Gimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MoveElapsedTime += DeltaTime;
		const float Alpha = (LowerDuration > 0.f) ? FMath::Clamp(MoveElapsedTime / LowerDuration, 0.f, 1.f) : 1.f;
		SetActorLocation(FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha));

		if (Alpha >= 1.f)
		{
			bIsMoving = false;
			OnMovementFinished.Broadcast();
		}
	}
}

void AS1TogglePillar_Gimmick::OnRep_IsActivated()
{
	// 클라에서는 초기 리플리케이션 시점에 이 함수가 BeginPlay보다 먼저 불릴 수 있다 — 그때는
	// InitialLocation이 아직 캡처 전(ZeroVector)이라 엉뚱한 좌표로 애니메이션을 시작해버린다.
	// BeginPlay가 곧 올바른 InitialLocation 기준으로 순간 스냅을 해줄 것이므로 여기서는 건너뛴다.
	if (false == HasActorBegunPlay())
	{
		return;
	}

	// 전환 지점(서버 ActivateGimmick/ResetGimmick·클라 OnRep 공통).
	StartTransition(false == IsUsable(), /*bInstant=*/false);
}

void AS1TogglePillar_Gimmick::StartTransition(bool bLower, bool bInstant)
{
	MoveStartLocation = GetActorLocation();
	MoveTargetLocation = bLower ? (InitialLocation - FVector(0.f, 0.f, LowerHeight)) : InitialLocation;

	if (bInstant)
	{
		SetActorLocation(MoveTargetLocation);
		bIsMoving = false;
		return;
	}

	MoveElapsedTime = 0.f;
	bIsMoving = true;
}