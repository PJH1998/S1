// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/Branch/S1MontageProgression_RepeatLoopBranch.h"

void US1MontageProgression_RepeatLoopBranch::OnInputReleased()
{
	if (bMinOneLoop && LoopCount < 1)
	{
		bPendingKeyRelease = true;
		return;
	}
	ExitLoop(SelectEndSectionName(), bImmediateExitOnKeyUp);
}

FName US1MontageProgression_RepeatLoopBranch::SelectEndSectionName() const
{
	return EndSectionName;
}