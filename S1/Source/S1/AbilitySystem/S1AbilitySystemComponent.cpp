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

		if (false == TagToSpecHandles.Contains(AbilitySet.AbilityTag))
		{
			TagToSpecHandles.Emplace(AbilitySet.AbilityTag, Handle);
		}
		else
		{
			LOG_FATAL(TEXT("Add to Duplicate Tag : [%s]."), *AbilitySet.AbilityTag.ToString());
		}
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

void US1AbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpecHandle* SpecHandle = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == SpecHandle)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(*SpecHandle);
	if (nullptr == Spec)
	{
		return;
	}

	if (true == Spec->IsActive())
	{
		US1GameplayAbility* GA = Cast<US1GameplayAbility>(Spec->GetPrimaryInstance());
		if (nullptr != GA)
		{
			GA->OnInputReactivated();
		}
	}
	else
	{
		TryActivateAbility(*SpecHandle);
	}
}
