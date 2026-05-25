// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1PoolingData.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FS1PoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag PoolTag;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere)
	int32 Count = 10;
};

USTRUCT(BlueprintType)
struct FS1PoolSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FS1PoolEntry> Pools;
};

UCLASS()
class S1_API US1PoolingData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FS1PoolSet* FindByWorldTag(FGameplayTag WorldTag) const;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FS1PoolSet> WorldPools;
};
