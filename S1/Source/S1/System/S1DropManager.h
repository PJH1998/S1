// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Monster/S1DropItemResource.h"
#include "Data/Monster/S1MonsterDropTable.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1DropManager.generated.h"

class AController;
class AS1Monster;

UCLASS()
class S1_API US1DropManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void HandleMonsterDeath(AS1Monster* DeadMonster);
	void HandleMonsterDeathForOwners(AS1Monster* DeadMonster, const TArray<AController*>& RewardOwners);

private:
	void SpawnDropsForOwner(const US1MonsterDropTable& DropTable, FVector OriginLocation, AController* OwnerController);
	void SpawnDropItem(ES1DropItemType DropType, int32 Amount, FGameplayTag ItemTag, FGameplayTag RarityTag, FVector OriginLocation, AController* OwnerController);

	const FS1DropItemEntry* SelectDropItem(const FS1DropPool& DropPool) const;
	bool ShouldDropPool(const FS1DropPool& DropPool) const;
};
