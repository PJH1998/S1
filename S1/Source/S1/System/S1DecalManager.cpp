// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1DecalManager.h"
#include "Effect/Decal/S1Decal.h"
#include "Engine/World.h"
#include "System/S1PoolingManager.h"

US1DecalManager* US1DecalManager::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		return World->GetSubsystem<US1DecalManager>();
	}

	return nullptr;
}

void US1DecalManager::Deinitialize()
{
	Clear();
	Super::Deinitialize();
}

void US1DecalManager::Tick(float DeltaTime)
{
	for (int32 Index = ActiveDecals.Num() - 1; Index >= 0; --Index)
	{
		AS1Decal* Decal = ActiveDecals[Index];
		if (Decal == nullptr)
		{
			ActiveDecals.RemoveAtSwap(Index);
			continue;
		}

		if (Decal->UpdateDecal(DeltaTime))
		{
			ActiveDecals.RemoveAtSwap(Index);
			Decal->ReturnSelf();
		}
	}
}

TStatId US1DecalManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(US1DecalManager, STATGROUP_Tickables);
}

AS1Decal* US1DecalManager::ShowDecal(FGameplayTag PoolTag, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	US1PoolingManager* PoolingManager = World->GetSubsystem<US1PoolingManager>();
	if (PoolingManager == nullptr)
	{
		return nullptr;
	}

	AS1Decal* Decal = Cast<AS1Decal>(PoolingManager->SpawnFromPool(PoolTag, Location, Rotation));
	if (Decal == nullptr)
	{
		return nullptr;
	}

	ActiveDecals.AddUnique(Decal);

	return Decal;
}

void US1DecalManager::HideDecal(AS1Decal* Decal)
{
	if (Decal == nullptr)
	{
		return;
	}

	ActiveDecals.RemoveSingleSwap(Decal);

	if (false == IsValid(Decal))
	{
		return;
	}

	// 풀 반납은 PoolingManager 소유. ReturnSelf -> OnReturnToPool(HideDecal)로 정리됨.
	Decal->ReturnSelf();
}

void US1DecalManager::Clear()
{
	// 풀 액터의 파괴는 PoolingManager가 소유한다. 여기서는 활성 추적만 비운다.
	ActiveDecals.Reset();
}
