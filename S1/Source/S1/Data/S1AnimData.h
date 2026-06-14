// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1AnimData.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FS1MontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// _Sequence 전용: 콤보 섹션 순서 (e.g. ["Hit1", "Hit2", "Hit3"])
	// _Loop / _HoldLoop 계열은 비워둠 — 섹션명을 Progression UPROPERTY로 직접 관리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> SectionNames;
};

UCLASS()
class S1_API US1AnimData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FS1MontageData* FindMontageByTag(const FGameplayTag& MontageTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
	TMap<FGameplayTag, FS1MontageData> Montages;
};
