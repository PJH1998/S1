// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1PoolingManager.h"
#include "System/S1AssetManager.h"
#include "System/S1WorldSettings.h"
#include "Data/S1PoolingData.h"
#include "Engine/World.h"
#include "Interface/S1PoolingInterface.h"
#include "S1LogChannels.h"

void US1PoolingManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// 권위(서버)면 모든 풀을, 클라면 로컬(bLocalCosmetic) 풀만 채운다.
	// WorldSettings/AssetManager는 클라에서도 접근 가능하므로 net-mode로 early-return하지 않는다.
	AS1WorldSettings* WS = Cast<AS1WorldSettings>(InWorld.GetWorldSettings());
	if (false == IsValid(WS) || false == WS->PoolAssetTag.IsValid() || false == WS->PoolWorldTag.IsValid())
	{
		return;
	}

	AddToPoolFromAsset(WS->PoolAssetTag, WS->PoolWorldTag);
}

void US1PoolingManager::Deinitialize()
{
	for (auto& Pair : Pool)
	{
		for (AActor* Actor : Pair.Value)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
	}

	Pool.Empty();
	WorldToPoolTags.Empty();
	LocalPoolTags.Empty();

	// Active 추적
	// ActiveActors.Empty();

	Super::Deinitialize();
}

void US1PoolingManager::AddToPoolFromAsset(FGameplayTag AssetTag, FGameplayTag WorldTag)
{
	US1PoolingData* PoolingData = US1AssetManager::GetAssetByTag<US1PoolingData>(AssetTag);
	if (false == IsValid(PoolingData))
	{
		return;
	}

	const FS1PoolSet* PoolSet = PoolingData->FindByWorldTag(WorldTag);
	if (nullptr == PoolSet)
	{
		return;
	}

	const bool bIsAuthority = (GetWorld()->GetAuthGameMode() != nullptr);

	TArray<FGameplayTag>& PoolTags = WorldToPoolTags.FindOrAdd(WorldTag);
	for (const FS1PoolEntry& Entry : PoolSet->Pools)
	{
		if (false == Entry.PoolTag.IsValid() || nullptr == Entry.ActorClass)
		{
			continue;
		}

		// 비-로컬(권위-리플리케이트) 풀은 서버에서만 채운다.
		if (false == Entry.bLocalCosmetic && false == bIsAuthority)
		{
			continue;
		}

		if (Entry.bLocalCosmetic)
		{
			LocalPoolTags.Add(Entry.PoolTag);
		}

		AddToPool(Entry.PoolTag, Entry.ActorClass, Entry.Count);
		PoolTags.AddUnique(Entry.PoolTag);
	}
}

void US1PoolingManager::AddToPool(FGameplayTag PoolTag, TSubclassOf<AActor> ActorClass, int32 Count)
{
	TArray<AActor*>& ActorPool = Pool.FindOrAdd(PoolTag);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < Count; ++i)
	{
		AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (false == IsValid(Actor))
		{
			continue;
		}

		IS1PoolingInterface* Poolable = Cast<IS1PoolingInterface>(Actor);
		if (nullptr == Poolable)
		{
			LOG_WARNING(TEXT("PoolingManager: %s does not implement IS1PoolingInterface"), *Actor->GetName());
			Actor->Destroy();
			continue;
		}

		Poolable->OnReturnToPool();
		ActorPool.Add(Actor);
	}
}

AActor* US1PoolingManager::SpawnFromPool(FGameplayTag PoolTag, FVector Location, FRotator Rotation)
{
	// 로컬 풀은 클라에서도 허용. 비-로컬은 권위에서만.
	if (false == IsLocalPoolTag(PoolTag) && nullptr == GetWorld()->GetAuthGameMode())
	{
		LOG(TEXT("[Pool] SpawnFromPool: client call ignored | Tag=%s"), *PoolTag.ToString());
		return nullptr;
	}

	TArray<AActor*>* ActorPool = Pool.Find(PoolTag);
	if (nullptr == ActorPool || ActorPool->IsEmpty())
	{
		return nullptr;
	}

	AActor* Actor = ActorPool->Pop();

	IS1PoolingInterface* Poolable = Cast<IS1PoolingInterface>(Actor);
	if (false == IsValid(Actor) || nullptr == Poolable)
	{
		return nullptr;
	}

	Poolable->OnSpawnFromPool(PoolTag, Location, Rotation);

	// Active 추적
	// ActiveActors.FindOrAdd(PoolTag).Add(Actor);

	return Actor;
}

void US1PoolingManager::ReturnToPool(AActor* Actor, FGameplayTag PoolTag)
{
	// 로컬 풀은 클라에서도 허용. 비-로컬은 권위에서만.
	if (false == IsLocalPoolTag(PoolTag) && nullptr == GetWorld()->GetAuthGameMode())
	{
		return;
	}

	if (false == IsValid(Actor))
	{
		return;
	}

	IS1PoolingInterface* Poolable = Cast<IS1PoolingInterface>(Actor);
	if (nullptr == Poolable)
	{
		return;
	}

	TArray<AActor*>& ActorPool = Pool.FindOrAdd(PoolTag);
	if (ActorPool.Contains(Actor))
	{
		LOG_WARNING(TEXT("[Pool] ReturnToPool: double return ignored | Tag=%s | Actor=%s"), *PoolTag.ToString(), *Actor->GetName());
		return;
	}

	Poolable->OnReturnToPool();

	ActorPool.Add(Actor);
}

bool US1PoolingManager::IsLocalPoolTag(FGameplayTag PoolTag) const
{
	return LocalPoolTags.Contains(PoolTag);
}

void US1PoolingManager::RemovePoolByWorld(FGameplayTag WorldTag)
{
	TArray<FGameplayTag>* PoolTags = WorldToPoolTags.Find(WorldTag);
	if (nullptr == PoolTags)
	{
		return;
	}

	for (const FGameplayTag& PoolTag : *PoolTags)
	{
		// Active 추적
		// if (TArray<TWeakObjectPtr<AActor>>* Actives = ActiveActors.Find(PoolTag))
		// {
		// 	for (TWeakObjectPtr<AActor>& Ptr : *Actives)
		// 	{
		// 		if (Ptr.IsValid()) { Ptr->Destroy(); }
		// 	}
		// 	ActiveActors.Remove(PoolTag);
		// }

		if (TArray<AActor*>* ActorPool = Pool.Find(PoolTag))
		{
			for (AActor* Actor : *ActorPool)
			{
				if (IsValid(Actor))
				{
					Actor->Destroy();
				}
			}
			Pool.Remove(PoolTag);
		}

		LocalPoolTags.Remove(PoolTag);
	}

	WorldToPoolTags.Remove(WorldTag);
}
