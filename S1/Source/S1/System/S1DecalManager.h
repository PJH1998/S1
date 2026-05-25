// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Effect/Decal/S1Decal_AttackRange.h"
#include "S1DecalManager.generated.h"

class AS1Decal;

UCLASS()
class S1_API US1DecalManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	static US1DecalManager* Get(const UObject* WorldContextObject);

public:
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Decal")
	AS1Decal_AttackRange* ShowAttackRangeDecal(const FS1AttackRangeDecalRequest& Request);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HideDecal(AS1Decal* Decal);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void Clear();

private:
	TSubclassOf<AS1Decal_AttackRange> GetAttackRangeDecalClass() const;
	AS1Decal* AcquireDecal(TSubclassOf<AS1Decal> DecalClass);
	void ReleaseDecal(AS1Decal* Decal);

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1Decal>> ActiveDecals;

	TMap<TSubclassOf<AS1Decal>, TArray<TWeakObjectPtr<AS1Decal>>> InactiveDecalPools;
};
