// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Combo/S1GameplayAbility_ComboAttack.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "Character/S1Character.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"

void US1GameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentSectionIndex = 0;
	PlayMontageSet(ActorInfo);
}

void US1GameplayAbility_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentSectionIndex = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_ComboAttack::OnInputReactivated()
{
	return TryAdvanceCombo();
}

bool US1GameplayAbility_ComboAttack::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
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
	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	return true;
}

bool US1GameplayAbility_ComboAttack::CanNextAction(UAbilitySystemComponent* ASC) const
{
	return ASC->HasMatchingGameplayTag(CanNextAttackTag);
}

const FS1MontageSet* US1GameplayAbility_ComboAttack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (false == IsValid(AnimData))
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag, CurrentSectionIndex);
}

void US1GameplayAbility_ComboAttack::PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo)
{
	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AS1Character* Character = Cast<AS1Character>(ActorInfo->AvatarActor);
	if (false == IsValid(Character))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Character->PlayAnimMontage(MontageSet->Montage);

	US1AnimInstance* AnimInst = GetAnimInstance();
	if (false == IsValid(AnimInst))
	{
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
}

bool US1GameplayAbility_ComboAttack::TryAdvanceCombo()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false; // 전환 불가 — 호출부에서 큐잉 처리
	}

	const FS1MontageData* MontageData = GetMontageData();
	if (nullptr == MontageData)
	{
		return false;
	}

	int32 NextIndex = CurrentSectionIndex + 1;
	if (false == MontageData->MontageSets.IsValidIndex(NextIndex))
	{
		// 마지막 콤보 → NextAttackAbilityTag로 전이
		if (NextAttackAbilityTag.IsValid())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(NextAttackAbilityTag));
		}
		return true;
	}

	CurrentSectionIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(CanNextAttackTag);
	PlayMontageSet(CurrentActorInfo);

	if (bRotateToCamera)
	{
		StartRotateToCamera();
	}

	return true;
}

void US1GameplayAbility_ComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* CurrentSet = GetCurrentMontageSet();
	if (nullptr == CurrentSet || Montage != CurrentSet->Montage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}
