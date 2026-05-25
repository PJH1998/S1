// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1PoolingData.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FS1PoolSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag PoolTag;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere)
	int32 Count = 10;
};

UCLASS()
class S1_API US1PoolingData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FS1PoolSet> Pools;
};
