// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "Animation/S1PlayerAnimInstance.h"
#include "Character/S1Character.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"

US1GameplayAbility_Attack::US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentSectionIndex = 0;
	PlayMontageSet(ActorInfo);
}

void US1GameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentSectionIndex = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Attack::OnInputReactivated()
{
	TryAdvanceCombo();
}

void US1GameplayAbility_Attack::PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo)
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

	OnBeforePlayMontage();

	Character->PlayAnimMontage(MontageSet->Montage);

	US1PlayerAnimInstance* AnimInst = GetPlayerAnimInstance();
	if (nullptr == AnimInst)
	{
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &US1GameplayAbility_Attack::OnMontageEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
}

void US1GameplayAbility_Attack::TryAdvanceCombo()
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
		return;
	}

	CurrentSectionIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(S1StateTags::State_CanNextAttack);

	PlayMontageSet(CurrentActorInfo);
}

void US1GameplayAbility_Attack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* CurrentSet = GetCurrentMontageSet();
	if (nullptr == CurrentSet || Montage != CurrentSet->Montage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

US1PlayerAnimInstance* US1GameplayAbility_Attack::GetPlayerAnimInstance() const
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (nullptr == Character)
	{
		return nullptr;
	}

	return Cast<US1PlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
}

const FS1MontageData* US1GameplayAbility_Attack::GetMontageData() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(MontageTag);
}

const FS1MontageSet* US1GameplayAbility_Attack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag, CurrentSectionIndex);
}
