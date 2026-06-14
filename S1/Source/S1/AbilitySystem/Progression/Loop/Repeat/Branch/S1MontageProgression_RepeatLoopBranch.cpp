// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/Branch/S1MontageProgression_RepeatLoopBranch.h"

void US1MontageProgression_RepeatLoopBranch::OnInputReleased()
{
	ExitLoop(SelectEndMontage(), bImmediateExitOnKeyUp);
}

UAnimMontage* US1MontageProgression_RepeatLoopBranch::SelectEndMontage() const
{
	return EndMontage.Get();
}
