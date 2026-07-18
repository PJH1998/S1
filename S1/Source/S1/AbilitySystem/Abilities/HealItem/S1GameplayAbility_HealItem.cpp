// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/HealItem/S1GameplayAbility_HealItem.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Character/Player/S1Player.h"

void US1GameplayAbility_HealItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(MontageProgression))
	{
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}
}

void US1GameplayAbility_HealItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// ServerOnly라 여기는 서버에서만 실행됨 — Multicast로 전체 머신(소유 클라 포함)의 로컬 무기/아이템 상태를 정리
	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		Player->MulticastResetHealItemPresentation();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
