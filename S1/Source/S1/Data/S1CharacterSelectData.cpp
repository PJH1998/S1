// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1CharacterSelectData.h"

const FS1SelectCharacterEntry* US1CharacterSelectData::FindEntryByTag(const FGameplayTag& Tag) const
{
	return Entries.Find(Tag);
}
