// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1EffectData.generated.h"

class AS1Effect;

UCLASS()
class S1_API US1EffectData : public UDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<AS1Effect> FindEffectClassByTag(const FGameplayTag& EffectTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TMap<FGameplayTag, TSubclassOf<AS1Effect>> Effects;
};
