// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Sequence/S1MontageProgression_Sequence.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "GameFramework/Character.h"

void US1MontageProgression_Sequence::OnActivated()
{
	CurrentComboIndex = 0;
	PlayMontageAtIndex();
}

void US1MontageProgression_Sequence::OnDeactivated()
{
	CurrentComboIndex = 0;
}

bool US1MontageProgression_Sequence::OnInputReactivated()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	return TryAdvanceCombo();
}

bool US1MontageProgression_Sequence::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty() || false == TargetAbilityTags.HasAny(CrossComboGroupTags))
	{
		return false;
	}

	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	GA->RequestEndAbility(false);
	return true;
}

void US1MontageProgression_Sequence::PlayMontageAtIndex()
{
	if (false == GA.IsValid())
	{
		return;
	}

	if (false == ComboMontages.IsValidIndex(CurrentComboIndex) || false == IsValid(ComboMontages[CurrentComboIndex]))
	{
		GA->RequestEndAbility(true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	UAnimMontage* CurrentMontage = ComboMontages[CurrentComboIndex];

	Character->PlayAnimMontage(CurrentMontage);

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, CurrentMontage);
	}
}

bool US1MontageProgression_Sequence::TryAdvanceCombo()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	const int32 NextIndex = CurrentComboIndex + 1;
	if (false == ComboMontages.IsValidIndex(NextIndex))
	{
		// 마지막 콤보 → NextAttackAbilityTag로 전이 or 자기 재발동
		if (NextAttackAbilityTag.IsValid())
		{
			GA->RequestEndAbility(false);
			GA->RequestActivateAbilityByTag(FGameplayTagContainer(NextAttackAbilityTag));
		}
		else
		{
			GA->RequestReactivateSelf();
		}
		return true;
	}

	CurrentComboIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(CanNextAttackTag);
	PlayMontageAtIndex();
	GA->OnProgressionMontageStarted();

	return true;
}

bool US1MontageProgression_Sequence::CanNextAction(UAbilitySystemComponent* ASC) const
{
	return ASC->HasMatchingGameplayTag(CanNextAttackTag);
}

void US1MontageProgression_Sequence::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (false == ComboMontages.IsValidIndex(CurrentComboIndex) || Montage != ComboMontages[CurrentComboIndex])
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
