// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1AnimData.h"

const FS1MontageData* US1AnimData::FindMontageByTag(const FGameplayTag& MontageTag) const
{
	return Montages.Find(MontageTag);
}
