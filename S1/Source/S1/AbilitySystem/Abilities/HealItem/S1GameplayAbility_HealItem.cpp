// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/HealItem/S1GameplayAbility_HealItem.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Character/Player/S1Player.h"

US1GameplayAbility_HealItem::US1GameplayAbility_HealItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 아이템 사용 판정은 서버 권위지만 몽타주는 소유 클라에서도 재생돼야 한다 (Hit_Weak과 동일한 이유)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

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
	// Multicast로 전체 머신(소유 클라 포함)의 로컬 무기/아이템 상태를 정리.
	// ServerInitiated라 이 함수가 소유 클라에서도 도므로 멀티캐스트는 서버에서만 — 클라가 부르면
	// 경고 없이 로컬 실행되어(FunctionCallspace::Local) 서버 멀티캐스트와 중복된다
	if (false == HasAuthority(&ActivationInfo))
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		Player->MulticastResetHealItemPresentation();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
