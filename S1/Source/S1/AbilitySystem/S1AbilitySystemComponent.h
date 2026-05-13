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
	void AddCharacterAbilities(const FGameplayTag& AssetTag);
	void RemoveCharacterAbilities(const FGameplayTag& AssetTag);
	void ActivateAbility(const FGameplayTag& AbilityTag);

private:
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> TagToSpecHandles;
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> GroupToSpecHandles;
};
