// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Loop/S1GameplayAbility_LoopAttack.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/S1AnimInstance.h"
#include "Data/S1AnimData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

US1GameplayAbility_LoopAttack::US1GameplayAbility_LoopAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GameplayAbility_LoopAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (StartSection.IsNone() || LoopSection.IsNone() || EndSection.IsNone())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (nullptr == Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	const float Duration = Character->PlayAnimMontage(MontageSet->Montage, 1.f, StartSection);
	if (Duration <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	US1AnimInstance* AnimInst = GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_SetNextSection(LoopSection, LoopSection, MontageSet->Montage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
	}

	if (LoopEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, LoopEndEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnLoopEndEventReceived);
		Task->ReadyForActivation();
	}
}

void US1GameplayAbility_LoopAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_LoopAttack::OnLoopEndEventReceived(FGameplayEventData Payload)
{
	OnLoopEndEvent(Payload);
}

void US1GameplayAbility_LoopAttack::OnLoopEndEvent(FGameplayEventData Payload)
{
	US1AnimInstance* AnimInst = GetAnimInstance();
	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (AnimInst && MontageSet)
	{
		//AnimInst->Montage_SetNextSection(LoopSection, EndSection, MontageSet->Montage);
		AnimInst->Montage_JumpToSection(EndSection);
	}
}

bool US1GameplayAbility_LoopAttack::OnInputReactivated()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (nullptr == ASC || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	if (Spec)
	{
		ASC->TryActivateAbilitiesByTag(Spec->Ability->AbilityTags);
	}
	return true;
}

bool US1GameplayAbility_LoopAttack::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(CrossComboGroupTags))
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (nullptr == ASC || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	return true;
}

FGameplayTag US1GameplayAbility_LoopAttack::GetInputFlushTag() const
{
	return CanNextAttackTag;
}

void US1GameplayAbility_LoopAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || Montage != MontageSet->Montage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}
