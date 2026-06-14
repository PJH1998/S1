// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/Branch/Count/S1MontageProgression_RepeatLoopBranch_Count.h"

UAnimMontage* US1MontageProgression_RepeatLoopBranch_Count::SelectEndMontage() const
{
	return (LoopCount >= Threshold) ? EndMontage_B.Get() : EndMontage.Get();
}
