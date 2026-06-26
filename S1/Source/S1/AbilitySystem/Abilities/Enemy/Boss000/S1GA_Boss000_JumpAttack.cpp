// Fill out your copyright notice in the Description page of Project Settings.

#include "S1GA_Boss000_JumpAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "Tags/S1GameplayTags.h"

US1GA_Boss000_JumpAttack::US1GA_Boss000_JumpAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_JumpAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageData* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(ActorInfo->AvatarActor);
	if (nullptr == Monster)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 Start 섹션부터 재생. 완료/중단 시 US1GA_Enemy가 EndAbility 처리.
	if (PlayAttackMontage(MontageSet->Montage) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 정점/착지 물리 이벤트로 몽타주 섹션을 전환한다.
	UAbilityTask_WaitGameplayEvent* ApexTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, S1EventTags::Event_Enemy_Apex, nullptr, true, true);
	ApexTask->EventReceived.AddDynamic(this, &ThisClass::OnApex);
	ApexTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* LandedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, S1EventTags::Event_Enemy_Landed, nullptr, true, true);
	LandedTask->EventReceived.AddDynamic(this, &ThisClass::OnLanded);
	LandedTask->ReadyForActivation();
}

void US1GA_Boss000_JumpAttack::OnApex(FGameplayEventData Payload)
{
	// ASC를 통해 섹션 점프 -> RepAnimMontageInfo로 클라이언트에도 복제된다.
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->CurrentMontageJumpToSection(TopSectionName);
	}
}

void US1GA_Boss000_JumpAttack::OnLanded(FGameplayEventData Payload)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->CurrentMontageJumpToSection(EndSectionName);
	}
}