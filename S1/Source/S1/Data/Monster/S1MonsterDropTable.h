// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1MonsterDropTable.generated.h"

USTRUCT(BlueprintType)
struct FS1CommonRewardData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinGold = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxGold = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Exp = {};
};

USTRUCT(BlueprintType)
struct FS1DropItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag RarityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32		MinCount = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32		MaxCount = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float		Weight = {};
};

USTRUCT(BlueprintType)
struct FS1DropPool
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName	PoolName = {};

	// Pool 드랍 확률
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float		DropChance = {};

	// Pool 드랍 횟수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32		DropCount = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FS1DropItemEntry>		DropItems;
};

UCLASS()
class S1_API US1MonsterDropTable : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Common")
	FS1CommonRewardData		CommonReward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Random")
	TArray<FS1DropPool>			DropPools;
};
