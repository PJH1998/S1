// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1DecalManager.h"
#include "Data/S1EffectData.h"
#include "Effect/Decal/S1Decal.h"
#include "Engine/World.h"
#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"

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
			ReleaseDecal(Decal);
		}
	}
}

TStatId US1DecalManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(US1DecalManager, STATGROUP_Tickables);
}

AS1Decal_AttackRange* US1DecalManager::ShowAttackRangeDecal(const FS1AttackRangeDecalRequest& Request)
{
	TSubclassOf<AS1Decal_AttackRange> DecalClass = GetAttackRangeDecalClass();
	if (DecalClass == nullptr)
	{
		DecalClass = AS1Decal_AttackRange::StaticClass();
	}

	AS1Decal_AttackRange* Decal = Cast<AS1Decal_AttackRange>(AcquireDecal(DecalClass));
	if (Decal == nullptr)
	{
		return nullptr;
	}

	Decal->ShowAttackRange(Request);
	ActiveDecals.AddUnique(Decal);

	return Decal;
}

TSubclassOf<AS1Decal_AttackRange> US1DecalManager::GetAttackRangeDecalClass() const
{
	const US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(S1AssetTags::Asset_Effect);
	if (EffectData == nullptr)
	{
		return nullptr;
	}

	const TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(S1EffectTags::Effect_Decal_AttackRange);
	if (EffectClass == nullptr)
	{
		return nullptr;
	}

	if (false == EffectClass->IsChildOf(AS1Decal_AttackRange::StaticClass()))
	{
		UE_LOG(LogWindows, Error, TEXT("EffectClass [%s] is not child of AS1Decal_AttackRange."), *EffectClass->GetName());
		return nullptr;
	}

	return TSubclassOf<AS1Decal_AttackRange>(EffectClass.Get());
}

void US1DecalManager::HideDecal(AS1Decal* Decal)
{
	ReleaseDecal(Decal);
}

void US1DecalManager::Clear()
{
	for (AS1Decal* Decal : ActiveDecals)
	{
		if (IsValid(Decal))
		{
			Decal->Destroy();
		}
	}
	ActiveDecals.Reset();

	for (TPair<TSubclassOf<AS1Decal>, TArray<TWeakObjectPtr<AS1Decal>>>& PoolPair : InactiveDecalPools)
	{
		for (TWeakObjectPtr<AS1Decal>& DecalPtr : PoolPair.Value)
		{
			if (AS1Decal* Decal = DecalPtr.Get())
			{
				Decal->Destroy();
			}
		}
	}
	InactiveDecalPools.Reset();
}

AS1Decal* US1DecalManager::AcquireDecal(TSubclassOf<AS1Decal> DecalClass)
{
	if (DecalClass == nullptr || GetWorld() == nullptr)
	{
		return nullptr;
	}

	TArray<TWeakObjectPtr<AS1Decal>>& Pool = InactiveDecalPools.FindOrAdd(DecalClass);
	while (Pool.Num() > 0)
	{
		TWeakObjectPtr<AS1Decal> DecalPtr = Pool.Pop(EAllowShrinking::No);
		if (AS1Decal* Decal = DecalPtr.Get())
		{
			return Decal;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AS1Decal* NewDecal = GetWorld()->SpawnActor<AS1Decal>(DecalClass, FTransform::Identity, SpawnParams);
	if (NewDecal)
	{
		NewDecal->HideDecal();
	}

	return NewDecal;
}

void US1DecalManager::ReleaseDecal(AS1Decal* Decal)
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

	Decal->HideDecal();
	InactiveDecalPools.FindOrAdd(Decal->GetClass()).Add(Decal);
}
