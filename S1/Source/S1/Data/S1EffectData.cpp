// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1EffectData.h"
#include "S1/Effect/S1Effect.h"

TSubclassOf<AS1Effect> US1EffectData::FindEffectClassByTag(const FGameplayTag& EffectTag) const
{
	if (const TSubclassOf<AS1Effect>* Found = Effects.Find(EffectTag))
	{
		return *Found;
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find EffectClass for EffectTag [%s]"), *EffectTag.ToString());

	return nullptr;
}
