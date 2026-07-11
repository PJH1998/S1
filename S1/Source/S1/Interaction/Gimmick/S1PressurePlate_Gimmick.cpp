// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1PressurePlate_Gimmick.h"

#include "Character/Player/S1Player.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1Define.h"

AS1PressurePlate_Gimmick::AS1PressurePlate_Gimmick()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(false);

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	SetRootComponent(PlateMesh);
	PlateMesh->SetGenerateOverlapEvents(false);
	PlateMesh->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Block);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(PlateMesh);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);
}

void AS1PressurePlate_Gimmick::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	// 현재 복제 상태에 맞춰 초기 위치 스냅(late-join 클라 포함).
	StartTransition(false == IsUsable(), /*bInstant=*/true);

	if (HasAuthority())
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerVolumeBeginOverlap);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnTriggerVolumeEndOverlap);
	}
}

void AS1PressurePlate_Gimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MoveElapsedTime += DeltaTime;
		const float Alpha = (DescendDuration > 0.f) ? FMath::Clamp(MoveElapsedTime / DescendDuration, 0.f, 1.f) : 1.f;
		SetActorLocation(FMath::Lerp(MoveStartLocation, MoveTargetLocation, Alpha));

		if (Alpha >= 1.f)
		{
			bIsMoving = false;
		}
	}

	if (HasAuthority() && IsUsable() && OverlappingPlayers.Num() > 0)
	{
		TryActivateFromOverlap();
	}
}

void AS1PressurePlate_Gimmick::OnRep_IsActivated()
{
	// 전환 지점(서버 ActivateGimmick/ResetGimmick·클라 OnRep 공통).
	StartTransition(false == IsUsable(), /*bInstant=*/false);
}

void AS1PressurePlate_Gimmick::OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == HasAuthority())
	{
		return;
	}

	AS1Player* Player = Cast<AS1Player>(OtherActor);
	if (nullptr == Player)
	{
		return;
	}

	// 이 시점의 CurrentFloor는 아직 이번 프레임의 FindFloor 갱신 이전 값이라 여기서 바로 검증하지 않는다.
	// 겹침 목록에만 추가하고, 실제 바닥 검증은 Tick에서 한 프레임 늦게 수행한다.
	OverlappingPlayers.AddUnique(Player);
}

void AS1PressurePlate_Gimmick::OnTriggerVolumeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (AS1Player* Player = Cast<AS1Player>(OtherActor))
	{
		OverlappingPlayers.RemoveSingleSwap(Player);
	}
}

void AS1PressurePlate_Gimmick::TryActivateFromOverlap()
{
	for (const TWeakObjectPtr<AS1Player>& WeakPlayer : OverlappingPlayers)
	{
		AS1Player* Player = WeakPlayer.Get();
		if (nullptr == Player)
		{
			continue;
		}

		// 오버랩만으론 점프로 스치거나 옆에서 겹치는 오탐이 가능해서, 실제로 이 발판 상판을 딛고 서 있는지까지 확인.
		const UCharacterMovementComponent* MovementComponent = Player->GetCharacterMovement();
		if (nullptr == MovementComponent || false == MovementComponent->CurrentFloor.bBlockingHit || MovementComponent->CurrentFloor.HitResult.GetComponent() != PlateMesh)
		{
			continue;
		}

		ActivateGimmick();
		OnPlateTriggered.Broadcast(this);
		return;
	}
}

void AS1PressurePlate_Gimmick::StartTransition(bool bDescend, bool bInstant)
{
	MoveStartLocation = GetActorLocation();
	MoveTargetLocation = bDescend ? (InitialLocation - FVector(0.f, 0.f, DescendHeight)) : InitialLocation;

	if (bInstant)
	{
		SetActorLocation(MoveTargetLocation);
		bIsMoving = false;
		return;
	}

	MoveElapsedTime = 0.f;
	bIsMoving = true;
}
