// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1QuestData.h"

const FS1QuestSet* US1QuestData::FindByStageTag(FGameplayTag StageTag) const
{
	return StageQuests.Find(StageTag);
}
