// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
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
	AS1Decal* ShowDecal(FGameplayTag PoolTag, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void HideDecal(AS1Decal* Decal);

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void Clear();

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1Decal>> ActiveDecals;
};
