// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Directional/S1MontageProgression_Directional.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Animation/S1AnimInstance.h"

void US1MontageProgression_Directional::OnActivated()
{
	ActiveMontage = nullptr;

	if (false == GA.IsValid())
	{
		return;
	}

	TObjectPtr<UAnimMontage>* Found = DirectionalMontages.Find(PendingDirection);
	if (nullptr == Found || false == IsValid(*Found))
	{
		GA->RequestEndAbility(true);
		return;
	}

	ActiveMontage = *Found;

	// ASC 경유 재생 → 클라 복제
	const float Duration = GA->PlayAbilityMontage(ActiveMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, ActiveMontage);
	}
}

void US1MontageProgression_Directional::OnDeactivated()
{
	ActiveMontage = nullptr;
}

void US1MontageProgression_Directional::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
	if (InMontage != ActiveMontage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
