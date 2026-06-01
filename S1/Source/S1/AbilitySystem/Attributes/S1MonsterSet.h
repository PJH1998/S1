// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "S1MonsterSet.generated.h"

UCLASS()
class S1_API US1MonsterSet : public US1AttributeSet
{
	GENERATED_BODY()

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
