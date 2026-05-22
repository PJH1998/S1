// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "Data/S1AbilityData.h"

void US1AbilitySystemComponent::AddCharacterAbilities(const FGameplayTag& AssetTag)
{
	US1AbilityData* AbilityData = US1AssetManager::GetAssetByTag<US1AbilityData>(AssetTag);

	TArray<FGameplayAbilitySpecHandle>& GroupHandles = GroupToSpecHandles.FindOrAdd(AssetTag);

	for (auto& AbilitySet : AbilityData->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilitySet.Abilitiy, AbilitySet.AbilityLevel);
		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

		GroupHandles.Add(Handle);
		TagToSpecHandles.FindOrAdd(AbilitySet.AbilityTag).Add(Handle);
	}
}

void US1AbilitySystemComponent::RemoveCharacterAbilities(const FGameplayTag& AssetTag)
{
	TArray<FGameplayAbilitySpecHandle>* GroupHandles = GroupToSpecHandles.Find(AssetTag);
	if (nullptr == GroupHandles)
	{
		return;
	}

	for (auto& Handle : *GroupHandles)
	{
		ClearAbility(Handle);
	}

	US1AbilityData* AbilityData = US1AssetManager::GetAssetByTag<US1AbilityData>(AssetTag);
	for (auto& AbilitySet : AbilityData->Abilities)
	{
		TagToSpecHandles.Remove(AbilitySet.AbilityTag);
	}

	GroupToSpecHandles.Remove(AssetTag);
}

bool US1AbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return false;
	}

	bool bActivated = false;
	for (auto& Handle : *Handles)
	{
		bActivated |= TryActivateAbility(Handle);
	}

	return bActivated;
}
