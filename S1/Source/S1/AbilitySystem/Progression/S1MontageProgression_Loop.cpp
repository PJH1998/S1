// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/S1MontageProgression_Loop.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "Data/S1AnimData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1MontageProgression_Loop::OnActivated()
{
	if (false == GA.IsValid())
	{
		return;
	}

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		GA->RequestEndAbility(true);
		return;
	}

	if (StartSection.IsNone() || LoopSection.IsNone() || EndSection.IsNone())
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

	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	const float Duration = Character->PlayAnimMontage(MontageSet->Montage, 1.f, StartSection);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		AnimInst->Montage_SetNextSection(LoopSection, LoopSection, MontageSet->Montage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
	}

	if (LoopEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(GA.Get(), LoopEndEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnLoopEndEventReceived);
		Task->ReadyForActivation();
	}
}

bool US1MontageProgression_Loop::OnInputReactivated()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	GA->RequestReactivateSelf();
	return true;
}

bool US1MontageProgression_Loop::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(CrossComboGroupTags))
	{
		return false;
	}

	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	GA->RequestEndAbility(false);
	return true;
}

void US1MontageProgression_Loop::OnLoopEndEventReceived(FGameplayEventData Payload)
{
	if (false == GA.IsValid())
	{
		return;
	}

	// 서브클래스 GA hook (e.g. DiveAttack: 중력 리셋)
	GA->OnProgressionLoopEnded();

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		AnimInst->Montage_JumpToSection(EndSection);
	}
}

void US1MontageProgression_Loop::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || Montage != MontageSet->Montage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
