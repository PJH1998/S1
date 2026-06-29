// Fill out your copyright notice in the Description page of Project Settings.

#include "S1GA_Boss000_Guard.h"

#include "AbilitySystemComponent.h"
#include "Character/Boss/Boss000/S1Boss_000.h"
#include "Data/S1AnimData.h"
#include "Tags/Anim/S1GameplayTags_Anim.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"

US1GA_Boss000_Guard::US1GA_Boss000_Guard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 페이즈별 가드 몽타주 분기: 현재 노출 무기(Axe=1페이즈, Sword=2페이즈)로 판정.
	AS1Boss_000* Boss = Cast<AS1Boss_000>(ActorInfo->AvatarActor);
	const bool bPhase2 = (Boss && Boss->GetActiveWeapon() == EBossWeapon::Sword);
	MontageTag = bPhase2 ? MontageTag_Phase02
	                     : MontageTag_Phase01;

	const FS1MontageData* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 가드 상태 진입: 정면 차단 판정(US1BossSet)이 읽는 태그를 부여한다.
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(S1StateTags::State_Boss_Guard);
	}

	// Animation (몽타주 종료 = 가드 윈도우 종료)
	if (false == PlayAttackMontage(MontageSet->Montage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void US1GA_Boss000_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 취소/중단 시에도 반드시 가드 태그를 제거한다.
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Boss_Guard);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
