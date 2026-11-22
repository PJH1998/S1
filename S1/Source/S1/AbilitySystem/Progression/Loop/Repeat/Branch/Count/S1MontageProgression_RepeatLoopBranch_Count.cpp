// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/Branch/Count/S1MontageProgression_RepeatLoopBranch_Count.h"

FName US1MontageProgression_RepeatLoopBranch_Count::SelectEndSectionName() const
{
	return (LoopCount >= Threshold) ? EndSectionName_B : EndSectionName;
}