// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1MontageProgression_Loop::OnActivated()
{
	bEndRequested  = false;
	CurrentEndMontage = EndMontage;

	if (false == GA.IsValid())
	{
		return;
	}

	if (false == IsValid(LoopMontage))
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

	if (IsValid(StartMontage))
	{
		const float Duration = Character->PlayAnimMontage(StartMontage);
		if (Duration <= 0.f)
		{
			GA->RequestEndAbility(true);
			return;
		}

		US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
		if (IsValid(AnimInst))
		{
			FOnMontageEnded StartDelegate;
			StartDelegate.BindUObject(this, &ThisClass::OnStartMontageEnded);
			AnimInst->Montage_SetEndDelegate(StartDelegate, StartMontage);
		}
	}
	else
	{
		PlayLoopMontage();
	}

	if (LoopEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(GA.Get(), LoopEndEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnLoopEndEventReceived);
		Task->ReadyForActivation();
	}
}

void US1MontageProgression_Loop::OnDeactivated()
{
	bEndRequested     = false;
	CurrentEndMontage = nullptr;
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
	if (CrossComboGroupTags.IsEmpty() || false == TargetAbilityTags.HasAny(CrossComboGroupTags))
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

void US1MontageProgression_Loop::ExitLoop(UAnimMontage* TargetEnd, bool bImmediate)
{
	if (bEndRequested)
	{
		return;
	}

	bEndRequested     = true;
	CurrentEndMontage = IsValid(TargetEnd) ? TargetEnd : EndMontage.Get();

	if (bImmediate)
	{
		US1AnimInstance* AnimInst = GA.IsValid() ? GA->GetAnimInstanceForProgression() : nullptr;
		if (IsValid(AnimInst))
		{
			AnimInst->Montage_Stop(0.0f, LoopMontage);
		}
		PlayEndMontage();
	}
	// else: OnLoopMontageEnded / OnStartMontageEnded 에서 처리
}

void US1MontageProgression_Loop::PlayLoopMontage()
{
	if (false == GA.IsValid())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	const float Duration = Character->PlayAnimMontage(LoopMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded LoopDelegate;
		LoopDelegate.BindUObject(this, &ThisClass::OnLoopMontageEnded);
		AnimInst->Montage_SetEndDelegate(LoopDelegate, LoopMontage);
	}
}

void US1MontageProgression_Loop::PlayEndMontage()
{
	if (false == GA.IsValid())
	{
		return;
	}

	if (false == IsValid(CurrentEndMontage))
	{
		GA->RequestEndAbility(false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	const float Duration = Character->PlayAnimMontage(CurrentEndMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnEndMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, CurrentEndMontage);
	}
}

void US1MontageProgression_Loop::OnStartMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != StartMontage)
	{
		return;
	}

	if (bEndRequested)
	{
		PlayEndMontage();
	}
	else
	{
		PlayLoopMontage();
	}
}

void US1MontageProgression_Loop::OnLoopMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != LoopMontage)
	{
		return;
	}

	if (bInterrupted)
	{
		// ExitLoop(bImmediate=true)에서 Montage_Stop 후 PlayEndMontage 이미 호출됨
		return;
	}

	OnLoopCycleCompleted();

	if (bEndRequested || ShouldExitLoop())
	{
		bEndRequested = true;
		PlayEndMontage();
	}
	else
	{
		if (LoopInertializationBlendTime > 0.f)
		{
			US1AnimInstance* AnimInst = GA.IsValid() ? GA->GetAnimInstanceForProgression() : nullptr;
			if (IsValid(AnimInst))
			{
				AnimInst->RequestInertialization(LoopInertializationBlendTime);
			}
		}
		PlayLoopMontage();
	}
}

void US1MontageProgression_Loop::OnEndMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CurrentEndMontage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}

void US1MontageProgression_Loop::OnLoopEndEventReceived(FGameplayEventData Payload)
{
	ExitLoop();
}
