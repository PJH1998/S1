// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1GameplayEffectData.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FS1GameplayEffectEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EffectTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;
};

UCLASS()
class S1_API US1GameplayEffectData : public UDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<UGameplayEffect> FindEffectClassByTag(const FGameplayTag& EffectTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffect")
	TArray<FS1GameplayEffectEntry> Effects;
};
