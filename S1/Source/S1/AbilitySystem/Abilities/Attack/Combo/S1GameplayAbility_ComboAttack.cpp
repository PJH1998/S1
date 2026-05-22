// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Combo/S1GameplayAbility_ComboAttack.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "Character/S1Character.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"

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

void US1GameplayAbility_ComboAttack::OnInputReactivated()
{
	TryAdvanceCombo();
}

const FS1MontageSet* US1GameplayAbility_ComboAttack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
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
	if (nullptr == Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Character->PlayAnimMontage(MontageSet->Montage);

	US1AnimInstance* AnimInst = GetAnimInstance();
	if (nullptr == AnimInst)
	{
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
}

void US1GameplayAbility_ComboAttack::TryAdvanceCombo()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (nullptr == ASC || false == ASC->HasMatchingGameplayTag(S1StateTags::State_CanNextAttack))
	{
		return;
	}

	const FS1MontageData* MontageData = GetMontageData();
	if (nullptr == MontageData)
	{
		return;
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
		return;
	}

	CurrentSectionIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(S1StateTags::State_CanNextAttack);
	PlayMontageSet(CurrentActorInfo);
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
