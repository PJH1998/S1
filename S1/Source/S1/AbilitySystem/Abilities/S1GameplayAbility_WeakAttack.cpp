// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/S1GameplayAbility_WeakAttack.h"
#include "Animation/S1PlayerAnimInstance.h"
#include "Character/S1Character.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"

US1GameplayAbility_WeakAttack::US1GameplayAbility_WeakAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GameplayAbility_WeakAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentSectionIndex = 0;
	PlayMontageSet(ActorInfo);
}

void US1GameplayAbility_WeakAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CurrentSectionIndex = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_WeakAttack::OnInputReactivated()
{
	TryAdvanceCombo();
}

void US1GameplayAbility_WeakAttack::PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo)
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

	US1PlayerAnimInstance* AnimInst = GetPlayerAnimInstance();
	if (nullptr == AnimInst)
	{
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &US1GameplayAbility_WeakAttack::OnMontageEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
}

void US1GameplayAbility_WeakAttack::TryAdvanceCombo()
{
	US1PlayerAnimInstance* AnimInst = GetPlayerAnimInstance();
	if (nullptr == AnimInst || false == AnimInst->bCanNextAttack)
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
	AnimInst->bCanNextAttack = false;

	PlayMontageSet(CurrentActorInfo);
}

void US1GameplayAbility_WeakAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* CurrentSet = GetCurrentMontageSet();
	if (nullptr == CurrentSet || Montage != CurrentSet->Montage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

US1PlayerAnimInstance* US1GameplayAbility_WeakAttack::GetPlayerAnimInstance() const
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (nullptr == Character)
	{
		return nullptr;
	}

	return Cast<US1PlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
}

const FS1MontageData* US1GameplayAbility_WeakAttack::GetMontageData() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(MontageTag);
}

const FS1MontageSet* US1GameplayAbility_WeakAttack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag, CurrentSectionIndex);
}
