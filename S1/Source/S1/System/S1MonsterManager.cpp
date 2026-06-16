// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1MonsterManager.h"
#include "Character/S1Monster.h"

void US1MonsterManager::RegisterMonster(AS1Monster* Monster)
{
	if (::IsValid(Monster))
	{
		ActiveMonsters.AddUnique(Monster);
	}
}

void US1MonsterManager::UnregisterMonster(AS1Monster* Monster)
{
	ActiveMonsters.RemoveSwap(Monster);
}
