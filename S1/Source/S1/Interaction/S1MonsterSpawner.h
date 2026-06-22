// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "S1MonsterSpawner.generated.h"

class AS1Monster;
class USceneComponent;
class USphereComponent;
class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FS1MonsterSpawnSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FGameplayTag PoolTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FTransform RelativeSpawnTransform = FTransform::Identity;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<AActor> SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
	float RespawnTime = 5.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<AS1Monster> ActiveMonster;

	bool bPendingRespawn = false;
	FTimerHandle RespawnTimerHandle;
};

UCLASS()
class S1_API AS1MonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AS1MonsterSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void ActivateSpawner();

protected:
	UFUNCTION()
	void OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMonsterReturnedToPool(AS1Monster* Monster);

private:
	void SpawnAllAvailableSlots();
	void SpawnSlot(int32 SlotIndex);
	void ScheduleRespawn(int32 SlotIndex);
	void HandleRespawnTimerElapsed(int32 SlotIndex);
	void SpawnPendingSlots();
	void CacheAlreadyOverlappingPlayers();
	bool CanSpawnByRange() const;
	bool IsPlayerActor(AActor* Actor) const;
	FTransform GetSpawnTransform(const FS1MonsterSpawnSlot& Slot) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<USphereComponent> DetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = "0.0"))
	float DetectionRadius = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bSpawnOnlyWhenPlayerInRange = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = "0.1"))
	float PoolRetryTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TArray<FS1MonsterSpawnSlot> SpawnSlots;

private:
	TSet<TWeakObjectPtr<AActor>> OverlappingPlayers;

	bool bHasActivated = false;
};
