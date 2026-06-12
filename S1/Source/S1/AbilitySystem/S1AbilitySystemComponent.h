// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "S1AbilitySystemComponent.generated.h"

UCLASS()
class S1_API US1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void AddCharacterAbilities(const FGameplayTag& AssetTag);
	virtual void RemoveCharacterAbilities(const FGameplayTag& AssetTag);
	virtual bool ActivateAbility(const FGameplayTag& AbilityTag);

	// 키 업 — 해당 태그의 활성 GA에 OnInputReleased 전달 (비활성이면 무시)
	void ReleaseAbility(const FGameplayTag& AbilityTag);

protected:
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> TagToSpecHandles;
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> GroupToSpecHandles;
};
