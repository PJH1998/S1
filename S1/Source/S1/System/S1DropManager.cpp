// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1DropManager.h"
#include "Character/S1Monster.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Item/S1DropItem.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "System/S1PoolingManager.h"

void US1DropManager::HandleMonsterDeath(AS1Monster* DeadMonster)
{
	UWorld* World = GetWorld();
	if (World == nullptr || World->GetAuthGameMode() == nullptr)
	{
		return;
	}

	TArray<AController*> RewardOwners;

	if (APlayerController* PlayerController = World->GetFirstPlayerController())
	{
		RewardOwners.Add(PlayerController);
	}

	HandleMonsterDeathForOwners(DeadMonster, RewardOwners);
}

void US1DropManager::HandleMonsterDeathForOwners(AS1Monster* DeadMonster, const TArray<AController*>& RewardOwners)
{
	UWorld* World = GetWorld();
	if (World == nullptr || World->GetAuthGameMode() == nullptr)
	{
		return;
	}

	if (false == IsValid(DeadMonster) || false == DeadMonster->GetDropTableTag().IsValid())
	{
		return;
	}

	US1MonsterDropTable* DropTable = US1AssetManager::GetAssetByTag<US1MonsterDropTable>(DeadMonster->GetDropTableTag());
	if (false == IsValid(DropTable))
	{
		return;
	}

	const FVector OriginLocation = DeadMonster->GetActorLocation() + FVector(0.f, 0.f, 40.f);
	for (AController* OwnerController : RewardOwners)
	{
		if (IsValid(OwnerController))
		{
			SpawnDropsForOwner(*DropTable, OriginLocation, OwnerController);
		}
	}
}

void US1DropManager::SpawnDropsForOwner(const US1MonsterDropTable& DropTable, FVector OriginLocation, AController* OwnerController)
{
	if (DropTable.CommonReward.MaxGold > 0)
	{
		const int32 MinGold = FMath::Max(DropTable.CommonReward.MinGold, 0);
		const int32 MaxGold = FMath::Max(DropTable.CommonReward.MaxGold, MinGold);
		const int32 GoldAmount = FMath::RandRange(MinGold, MaxGold);
		if (GoldAmount > 0)
		{
			SpawnDropItem(ES1DropItemType::Gold, GoldAmount, S1DropTags::Drop_Gold, FGameplayTag(), OriginLocation, OwnerController);
		}
	}

	if (DropTable.CommonReward.Exp > 0)
	{
		SpawnDropItem(ES1DropItemType::Exp, DropTable.CommonReward.Exp, S1DropTags::Drop_Exp, FGameplayTag(), OriginLocation, OwnerController);
	}

	for (const FS1DropPool& DropPool : DropTable.DropPools)
	{
		if (ShouldDropPool(DropPool) == false)
		{
			continue;
		}

		const int32 DropCount = FMath::Max(DropPool.DropCount, 0);
		for (int32 Count = 0; Count < DropCount; ++Count)
		{
			const FS1DropItemEntry* DropItem = SelectDropItem(DropPool);
			if (DropItem == nullptr)
			{
				continue;
			}

			const int32 MinCount = FMath::Max(DropItem->MinCount, 1);
			const int32 MaxCount = FMath::Max(DropItem->MaxCount, MinCount);
			SpawnDropItem(ES1DropItemType::Item, FMath::RandRange(MinCount, MaxCount), DropItem->ItemTag, DropItem->RarityTag, OriginLocation, OwnerController);
		}
	}
}

void US1DropManager::SpawnDropItem(ES1DropItemType DropType, int32 Amount, FGameplayTag ItemTag, FGameplayTag RarityTag, FVector OriginLocation, AController* OwnerController)
{
	if (Amount <= 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr || World->GetAuthGameMode() == nullptr)
	{
		return;
	}

	US1DropItemResource* DropItemResource = US1AssetManager::GetAssetByTag<US1DropItemResource>(S1AssetTags::Asset_DropItemResource);
	if (false == IsValid(DropItemResource))
	{
		return;
	}

	const FS1DropItemResourceEntry* Resource = DropItemResource->FindResource(DropType, ItemTag, RarityTag);
	if (Resource == nullptr)
	{
		return;
	}
	const FS1DropItemVisualParams& VisualParams = DropItemResource->FindVisualParams(DropType, RarityTag);

	US1PoolingManager* PoolingManager = World->GetSubsystem<US1PoolingManager>();
	if (false == IsValid(PoolingManager))
	{
		return;
	}

	AS1DropItem* DropItem = Cast<AS1DropItem>(PoolingManager->SpawnFromPool(S1PoolingTags::Pool_DropItem, OriginLocation, FRotator::ZeroRotator));
	if (false == IsValid(DropItem))
	{
		return;
	}

	DropItem->InitializeDrop(DropType, Amount, ItemTag, RarityTag, *Resource, VisualParams, OwnerController);
}

const FS1DropItemEntry* US1DropManager::SelectDropItem(const FS1DropPool& DropPool) const
{
	float TotalWeight = 0.f;
	for (const FS1DropItemEntry& DropItem : DropPool.DropItems)
	{
		TotalWeight += FMath::Max(DropItem.Weight, 0.f);
	}

	if (TotalWeight <= 0.f)
	{
		return nullptr;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	for (const FS1DropItemEntry& DropItem : DropPool.DropItems)
	{
		Roll -= FMath::Max(DropItem.Weight, 0.f);
		if (Roll <= 0.f)
		{
			return &DropItem;
		}
	}

	return DropPool.DropItems.IsEmpty() ? nullptr : &DropPool.DropItems.Last();
}

bool US1DropManager::ShouldDropPool(const FS1DropPool& DropPool) const
{
	const float DropChance = DropPool.DropChance > 1.f ? DropPool.DropChance * 0.01f : DropPool.DropChance;
	return FMath::FRand() <= FMath::Clamp(DropChance, 0.f, 1.f);
}
