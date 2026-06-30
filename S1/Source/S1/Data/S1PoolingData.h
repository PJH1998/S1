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

	// true면 클라 포함 전 net-mode에서 로컬 스폰/반납 (데칼 같은 비리플리케이트 비주얼)
	UPROPERTY(EditAnywhere)
	bool bLocalCosmetic = false;
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
