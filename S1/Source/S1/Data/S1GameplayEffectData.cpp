// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1GameplayEffectData.h"

TSubclassOf<UGameplayEffect> US1GameplayEffectData::FindEffectClassByTag(const FGameplayTag& EffectTag) const
{
	for (const FS1GameplayEffectEntry& Entry : Effects)
	{
		if (Entry.EffectTag == EffectTag)
		{
			return Entry.EffectClass;
		}
	}
	return nullptr;
}
