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

	void AddToPoolFromAsset(FGameplayTag AssetTag, FGameplayTag WorldTag);
	void RemovePoolByWorld(FGameplayTag WorldTag);

	AActor* SpawnFromPool(FGameplayTag PoolTag, FVector Location, FRotator Rotation);
	void ReturnToPool(AActor* Actor, FGameplayTag PoolTag);

	// 로컬(비리플리케이트) 풀 태그인지 — 권위 없이도 스폰/반납이 허용되는 풀.
	bool IsLocalPoolTag(FGameplayTag PoolTag) const;

private:
	void AddToPool(FGameplayTag PoolTag, TSubclassOf<AActor> ActorClass, int32 Count);

	TMap<FGameplayTag, TArray<AActor*>> Pool;

	TMap<FGameplayTag, TArray<FGameplayTag>> WorldToPoolTags;

	// bLocalCosmetic 엔트리의 PoolTag 집합 (클라 포함 전 net-mode에서 스폰/반납 허용).
	TSet<FGameplayTag> LocalPoolTags;

	// Active 추적
	// TMap<FGameplayTag, TArray<TWeakObjectPtr<AActor>>> ActiveActors;
};
