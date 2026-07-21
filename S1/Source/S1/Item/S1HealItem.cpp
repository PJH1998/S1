// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/S1HealItem.h"
#include "Component/S1DissolveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AS1HealItem::AS1HealItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// 각 머신이 Notify에서 로컬로 스폰하는 순수 비주얼 액터 — 복제 불필요
	bReplicates = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 소켓(손) 부착 시 위치만 따라가고 회전은 소켓(손 애니메이션) 영향을 받지 않도록 디커플링 — 회전은 RotatingMovementComponent가 전담
	ItemMesh->SetAbsolute(false, true, false);
	ItemMesh->SetRelativeRotation(FRotator::ZeroRotator);

	DissolveComponent = CreateDefaultSubobject<US1DissolveComponent>("DissolveComponent");

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovementComponent");
	RotatingMovementComponent->UpdatedComponent = ItemMesh;
	RotatingMovementComponent->bRotationInLocalSpace = true;
}

void AS1HealItem::PlaySpawnDissolve(float Duration)
{
	if (IsValid(DissolveComponent) && IsValid(ItemMesh))
	{
		DissolveComponent->PlayDissolve(ItemMesh, true, Duration);
	}
}

void AS1HealItem::PlayDespawnDissolve(float Duration)
{
	if (IsValid(DissolveComponent) && IsValid(ItemMesh))
	{
		DissolveComponent->OnDissolveComplete.AddUObject(this, &AS1HealItem::HandleDespawnDissolveComplete);
		DissolveComponent->PlayDissolve(ItemMesh, false, Duration);
	}
	else
	{
		Destroy();
	}
}

void AS1HealItem::SetRotationRate(const FRotator& RotationRate)
{
	if (IsValid(RotatingMovementComponent))
	{
		RotatingMovementComponent->RotationRate = RotationRate;
	}
}

void AS1HealItem::HandleDespawnDissolveComplete()
{
	Destroy();
}
