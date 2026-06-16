// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"

#include "AbilitySystemComponent.h"
#include "System/S1AssetManager.h"
#include "Data/S1AnimData.h"
#include "Tags/S1GameplayTags.h"

US1GA_Enemy::US1GA_Enemy(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GA_Enemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(S1StateTags::State_Enemy_Attacking);
	}
}

void US1GA_Enemy::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Enemy_Attacking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ActiveMontage = nullptr;
}

void US1GA_Enemy::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownEffectClass == nullptr)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffectClass, GetAbilityLevel(Handle, ActorInfo));

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);

		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

const FGameplayTagContainer* US1GA_Enemy::GetCooldownTags() const
{
	return &CooldownTags;
}

void US1GA_Enemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

const FS1MontageData* US1GA_Enemy::GetMontage() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(MontageTag);
}
