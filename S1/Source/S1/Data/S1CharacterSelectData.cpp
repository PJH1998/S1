// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1CharacterSelectData.h"
#include "Character/Player/S1Player.h"

UClass* US1CharacterSelectData::GetCharacterClass(const FGameplayTag& Tag) const
{
	const TSubclassOf<AS1Player>* Class = CharacterClasses.Find(Tag);
	if (nullptr == Class)
	{
		return nullptr;
	}
	return Class->Get();
}
