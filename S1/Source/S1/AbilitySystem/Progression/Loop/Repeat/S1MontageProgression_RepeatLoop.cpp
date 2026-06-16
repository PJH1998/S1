// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoop.h"

void US1MontageProgression_RepeatLoop::OnActivated()
{
	LoopCount          = 0;
	bPendingKeyRelease = false;
	Super::OnActivated();
}

void US1MontageProgression_RepeatLoop::OnDeactivated()
{
	LoopCount          = 0;
	bPendingKeyRelease = false;
	Super::OnDeactivated();
}

void US1MontageProgression_RepeatLoop::OnInputReleased()
{
	if (bMinOneLoop && LoopCount < 1)
	{
		bPendingKeyRelease = true;
		return;
	}
	ExitLoop(nullptr, bImmediateExitOnKeyUp);
}

void US1MontageProgression_RepeatLoop::OnLoopCycleCompleted()
{
	++LoopCount;
}

bool US1MontageProgression_RepeatLoop::ShouldExitLoop() const
{
	return bPendingKeyRelease || LoopCount >= MaxLoopCount;
}
