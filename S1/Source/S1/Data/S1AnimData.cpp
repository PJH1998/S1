// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1AnimData.h"

const FS1MontageData* US1AnimData::FindMontageByTag(const FGameplayTag& MontageTag) const
{
	return Montages.Find(MontageTag);
}

const FS1MontageSection* US1AnimData::FindSection(const FGameplayTag& MontageTag, const FName& SectionName) const
{
	const FS1MontageData* MontageData = Montages.Find(MontageTag);
	if (!MontageData)
		return nullptr;

	return MontageData->Sections.Find(SectionName);
}
