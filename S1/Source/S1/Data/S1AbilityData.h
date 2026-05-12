// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1AbilityData.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FS1AbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Abilitiy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;
};

UCLASS()
class S1_API US1AbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	bool FindAbilityByTag(const FGameplayTag& AbilityTag, OUT TSubclassOf<UGameplayAbility>& OutClass) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TArray<FS1AbilitySet> Abilities;
};
