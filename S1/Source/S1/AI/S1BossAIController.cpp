// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/S1BossAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

void AS1BossAIController::SetBossPhase(int32 PhaseIndex)
{
	if (BossPhaseKeyName == NAME_None)
	{
		return;
	}
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsInt(BossPhaseKeyName, PhaseIndex);
	}
}

void AS1BossAIController::SetPhaseTransitionPending(bool bPending)
{
	if (PhaseTransitionPendingKeyName == NAME_None)
	{
		return;
	}
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(PhaseTransitionPendingKeyName, bPending);
	}
}