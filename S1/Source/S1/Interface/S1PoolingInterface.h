// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "S1PoolingInterface.generated.h"

UINTERFACE(MinimalAPI)
class US1PoolingInterface : public UInterface
{
	GENERATED_BODY()
};

class S1_API IS1PoolingInterface
{
	GENERATED_BODY()

public:
	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation);
	virtual void OnReturnToPool();
	void ReturnSelf();

protected:
	FGameplayTag PoolTag;
};
