// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Sequence/S1MontageProgression_SequenceBranch.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Animation/S1AnimInstance.h"

void US1MontageProgression_SequenceBranch::OnActivated()
{
	CurrentEndMontage = nullptr;

	if (false == GA.IsValid())
	{
		return;
	}

	if (false == IsValid(BeginMontage))
	{
		GA->RequestEndAbility(true);
		return;
	}

	// ASC 경유 재생 → 클라 복제
	const float Duration = GA->PlayAbilityMontage(BeginMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}
}

void US1MontageProgression_SequenceBranch::OnDeactivated()
{
	CurrentEndMontage = nullptr;
}

void US1MontageProgression_SequenceBranch::OnBranchRequested(FGameplayTag InKey)
{
	if (false == GA.IsValid())
	{
		return;
	}

	TObjectPtr<UAnimMontage>* Found = EndMontages.Find(InKey);
	UAnimMontage* EndMontage = Found ? Found->Get() : nullptr;

	if (false == IsValid(EndMontage))
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (false == IsValid(AnimInst))
	{
		GA->RequestEndAbility(true);
		return;
	}


	CurrentEndMontage = EndMontage;

	// ASC 경유 재생 → 클라 복제
	const float Duration = GA->PlayAbilityMontage(EndMontage);
	if (Duration <= 0.f)
	{
		CurrentEndMontage = nullptr;
		GA->RequestEndAbility(true);
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnEndMontageEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate, EndMontage);
}

void US1MontageProgression_SequenceBranch::OnEndMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	if (InMontage != CurrentEndMontage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
