// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "S1AbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities);
	void ActivateAbility(FGameplayTag AbilityTag);

public:
	TArray<FGameplayAbilitySpecHandle> SpecHandles;
};
