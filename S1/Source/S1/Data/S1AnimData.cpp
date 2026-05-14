// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1AnimData.h"

const FS1MontageData* US1AnimData::FindMontageByTag(const FGameplayTag& MontageTag) const
{
	return Montages.Find(MontageTag);
}

const FS1MontageSet* US1AnimData::FindMontageSet(const FGameplayTag& MontageTag, int32 Index) const
{
	const FS1MontageData* MontageData = Montages.Find(MontageTag);
	if (nullptr == MontageData)
	{
		return nullptr;
	}

	if (false == MontageData->MontageSets.IsValidIndex(Index))
	{
		return nullptr;
	}

	return &MontageData->MontageSets[Index];
}
