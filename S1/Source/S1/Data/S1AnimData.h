// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1AnimData.generated.h"

class UAnimMontage;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FS1MontageSection
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SectionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageRatio = 1.f;
};

USTRUCT(BlueprintType)
struct FS1MontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FS1MontageSection> Sections;
};

UCLASS()
class S1_API US1AnimData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FS1MontageData* FindMontageByTag(const FGameplayTag& MontageTag) const;
	const FS1MontageSection* FindSection(const FGameplayTag& MontageTag, const FName& SectionName) const;

public:
	// MontageTag → MontageData
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	TMap<FGameplayTag, FS1MontageData> Montages;
};
