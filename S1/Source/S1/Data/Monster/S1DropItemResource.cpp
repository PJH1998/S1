// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Monster/S1DropItemResource.h"

const FS1DropItemResourceEntry* US1DropItemResource::FindResource(ES1DropItemType DropType, FGameplayTag ItemTag, FGameplayTag RarityTag) const
{
	const FS1DropItemResourceEntry* RarityMatch = nullptr;
	const FS1DropItemResourceEntry* TypeMatch = nullptr;

	for (const FS1DropItemResourceEntry& Resource : Resources)
	{
		if (Resource.DropType != DropType)
		{
			continue;
		}

		if (TypeMatch == nullptr)
		{
			TypeMatch = &Resource;
		}

		if (DropType != ES1DropItemType::Item)
		{
			return &Resource;
		}

		if (ItemTag.IsValid() && Resource.ItemTag == ItemTag)
		{
			return &Resource;
		}

		if (RarityMatch == nullptr && RarityTag.IsValid() && Resource.RarityTag == RarityTag)
		{
			RarityMatch = &Resource;
		}
	}

	return RarityMatch != nullptr ? RarityMatch : TypeMatch;
}

const FS1DropItemVisualParams& US1DropItemResource::FindVisualParams(ES1DropItemType DropType, FGameplayTag RarityTag) const
{
	switch (DropType)
	{
	case ES1DropItemType::Gold:
		return GoldVisual;
	case ES1DropItemType::Exp:
		return ExpVisual;
	case ES1DropItemType::Item:
	default:
		if (const FS1DropItemVisualParams* RarityVisual = RarityVisuals.Find(RarityTag))
		{
			return *RarityVisual;
		}
		return DefaultVisual;
	}
}
