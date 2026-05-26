// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1AnimData.generated.h"

class UAnimMontage;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FS1MontageSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAttackMontage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bAttackMontage", EditConditionHides))
	float DamageRatio = 1.f;
};

USTRUCT(BlueprintType)
struct FS1MontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FS1MontageSet> MontageSets;
};

UCLASS()
class S1_API US1AnimData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FS1MontageData* FindMontageByTag(const FGameplayTag& MontageTag) const;
	const FS1MontageSet* FindMontageSet(const FGameplayTag& MontageTag, int32 Index = 0) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	TMap<FGameplayTag, FS1MontageData> Montages;
};
