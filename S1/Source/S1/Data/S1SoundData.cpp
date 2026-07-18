// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1SoundData.h"
#include "Sound/SoundBase.h"

USoundBase* US1SoundData::FindSound(const FGameplayTag& SoundTag) const
{
	if (const TObjectPtr<USoundBase>* Found = Sounds.Find(SoundTag))
	{
		return *Found;
	}

	return nullptr;
}
