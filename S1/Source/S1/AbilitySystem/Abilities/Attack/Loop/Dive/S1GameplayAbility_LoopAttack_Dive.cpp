// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Loop/Dive/S1GameplayAbility_LoopAttack_Dive.h"

void US1GameplayAbility_LoopAttack_Dive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Start 구간: 공중 정지
	SetGravityScale(StartGravityScale);
}

void US1GameplayAbility_LoopAttack_Dive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetGravityScale();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_LoopAttack_Dive::OnProgressionLoopStarted()
{
	SetGravityScale(DiveGravityScale);
}

void US1GameplayAbility_LoopAttack_Dive::OnProgressionLoopEnded()
{
	ResetGravityScale();
}
