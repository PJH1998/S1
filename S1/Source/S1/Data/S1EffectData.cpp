// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1EffectData.h"

TSubclassOf<AS1Effect> US1EffectData::FindEffectClassByTag(const FGameplayTag& EffectTag) const
{
	for (const FS1EffectEntry& Effect : Effects)
	{
		if (Effect.EffectTag == EffectTag && Effect.EffectClass)
		{
			return Effect.EffectClass;
		}
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find EffectClass for EffectTag [%s]"), *EffectTag.ToString());

	return nullptr;
}
