// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GameplayTagContainer.h"
#include "S1WorldSettings.generated.h"

UCLASS()
class S1_API AS1WorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Pooling")
	FGameplayTag PoolAssetTag;
};
