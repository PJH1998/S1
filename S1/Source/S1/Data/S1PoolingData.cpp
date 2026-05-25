// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/S1PoolingData.h"

const FS1PoolSet* US1PoolingData::FindByWorldTag(FGameplayTag WorldTag) const
{
	return WorldPools.Find(WorldTag);
}
