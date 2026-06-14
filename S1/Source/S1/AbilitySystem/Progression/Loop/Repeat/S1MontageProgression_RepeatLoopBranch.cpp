// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoopBranch.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystemComponent.h"

void US1MontageProgression_RepeatLoopBranch::OnActivated()
{
	LoopCount = 0;
	Super::OnActivated();
}

void US1MontageProgression_RepeatLoopBranch::OnDeactivated()
{
	LoopCount = 0;
	Super::OnDeactivated();
}

void US1MontageProgression_RepeatLoopBranch::OnInputReleased()
{
	if (false == GA.IsValid() || false == BranchExitEventTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC))
	{
		return;
	}

	FGameplayEventData Payload;
	ASC->HandleGameplayEvent(BranchExitEventTag, &Payload);
}

void US1MontageProgression_RepeatLoopBranch::OnLoopCycleCompleted()
{
	++LoopCount;
}

void US1MontageProgression_RepeatLoopBranch::RequestEnd(FGameplayTag InKey)
{
	TObjectPtr<UAnimMontage>* Found = EndMontages.Find(InKey);
	ExitLoop(Found ? Found->Get() : nullptr);
}
