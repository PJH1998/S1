// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "Data/S1AbilityData.h"


void US1AbilitySystemComponent::AddCharacterAbilities(const FGameplayTag& AssetTag)
{
	US1AbilityData* AbilityData = US1AssetManager::GetAssetByTag<US1AbilityData>(AssetTag);

	for (auto& FS1AbilitySet : AbilityData->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(FS1AbilitySet.Abilitiy, FS1AbilitySet.AbilityLevel);
		
		FGameplayAbilitySpecHandle SpecHandle = GiveAbility(AbilitySpec);

		auto& a = ActivatableAbilities;

		//TryActivateAbility(SpecHandle);
		//GiveAbilityAndActivateOnce(AbilitySpec);

		if (TagToSpecHandles.Contains(FS1AbilitySet.AbilityTag) == false)
		{
			TagToSpecHandles.Emplace(FS1AbilitySet.AbilityTag, SpecHandle);
		}
		else
		{
			LOG_FATAL(TEXT("Add to Duplicate Tag : [%s]."), *(FS1AbilitySet.AbilityTag).ToString());
		}
	}
}

void US1AbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = TagToSpecHandles.Find(AbilityTag))
	{
		TryActivateAbility(*SpecHandle);

		LOG(TEXT("Active Ability"));
	}
}
