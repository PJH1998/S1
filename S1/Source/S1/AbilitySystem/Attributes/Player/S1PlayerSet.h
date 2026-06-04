// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "S1PlayerSet.generated.h"

UCLASS()
class S1_API US1PlayerSet : public US1AttributeSet
{
	GENERATED_BODY()

public:
	virtual void InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag) override;
};
