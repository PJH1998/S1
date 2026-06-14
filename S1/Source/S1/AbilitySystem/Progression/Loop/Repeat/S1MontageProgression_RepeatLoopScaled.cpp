// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoopScaled.h"

void US1MontageProgression_RepeatLoopScaled::OnActivated()
{
	CurrentStage = 0;
	Super::OnActivated();
}

void US1MontageProgression_RepeatLoopScaled::OnDeactivated()
{
	CurrentStage = 0;
	Super::OnDeactivated();
}

float US1MontageProgression_RepeatLoopScaled::GetDamageMultiplier() const
{
	if (DamageScaleStages.IsEmpty())
	{
		return 1.0f;
	}

	return DamageScaleStages[FMath::Clamp(CurrentStage, 0, DamageScaleStages.Num() - 1)];
}

void US1MontageProgression_RepeatLoopScaled::OnLoopCycleCompleted()
{
	Super::OnLoopCycleCompleted();  // ++LoopCount

	if (false == DamageScaleStages.IsEmpty())
	{
		CurrentStage = FMath::Min(LoopCount, DamageScaleStages.Num() - 1);
	}
}
