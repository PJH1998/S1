// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Data/S1AnimData.h"

void US1MontageProgression::Init(US1GameplayAbility_Action* InGA)
{
	GA = InGA;
}

const FS1MontageSet* US1MontageProgression::GetCurrentMontageSet() const
{
	if (false == GA.IsValid())
	{
		return nullptr;
	}

	const FS1MontageData* Data = GA->GetMontageData();
	if (nullptr == Data || Data->MontageSets.IsEmpty())
	{
		return nullptr;
	}

	return &Data->MontageSets[0];
}
