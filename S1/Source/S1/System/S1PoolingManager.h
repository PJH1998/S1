// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1PoolingManager.generated.h"

class AActor;

UCLASS()
class S1_API US1PoolingManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	void AddToPoolFromAsset(FGameplayTag AssetTag);
	void RemovePoolByAsset(FGameplayTag AssetTag);

	AActor* SpawnFromPool(FGameplayTag PoolTag, FVector Location, FRotator Rotation);
	void ReturnToPool(AActor* Actor, FGameplayTag PoolTag);

private:
	void AddToPool(FGameplayTag PoolTag, TSubclassOf<AActor> ActorClass, int32 Count);

	TMap<FGameplayTag, TArray<AActor*>> Pool;

	TMap<FGameplayTag, TArray<FGameplayTag>> AssetToPoolTags;

	// Active 추적
	// TMap<FGameplayTag, TArray<TWeakObjectPtr<AActor>>> ActiveActors;
};
