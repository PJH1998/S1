// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1AbilityData.h"
#include "Abilities/GameplayAbility.h"

bool US1AbilityData::FindAbilityByTag(const FGameplayTag& AbilityTag, OUT TSubclassOf<UGameplayAbility>& OutClass) const
{
	for (const FS1AbilitySet& AbilitySet : Abilities)
	{
		if (AbilitySet.Abilitiy && AbilitySet.AbilityTag == AbilityTag)
		{
			OutClass = AbilitySet.Abilitiy;
			return true;
		}
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find InputAction for InputTag [%s]"), *AbilityTag.ToString());

	return false;
}
