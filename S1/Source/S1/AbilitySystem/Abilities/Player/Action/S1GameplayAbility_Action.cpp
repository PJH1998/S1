// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"

void US1GameplayAbility_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 액션 중 상태 태그 부여 — Jump() 오버라이드에서 차단 조건으로 사용
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(ActionStateTag);
		}
	}

	// EarlyMove 이벤트 대기 등록
	if (EarlyMoveEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EarlyMoveEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnEarlyMoveEventReceived);
		Task->ReadyForActivation();
	}
}

void US1GameplayAbility_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(ActionStateTag);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Action::OnEarlyMoveEventReceived(FGameplayEventData Payload)
{
	OnEarlyMoveEnabled();
}

void US1GameplayAbility_Action::OnEarlyMoveEnabled()
{
	// 점프 차단 해제
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(ActionStateTag);
		}
	}

	// 이동/점프 입력 감지 시작 → TriggerInputDetected
	US1AbilityTask_EarlyExitChecker* Task = US1AbilityTask_EarlyExitChecker::Run(this, JumpEventTag);
	Task->OnInputDetected.AddUObject(this, &ThisClass::OnEarlyMoveTriggered);
	Task->ReadyForActivation();
}

void US1GameplayAbility_Action::OnEarlyMoveTriggered()
{
	if (CurrentActorInfo)
	{
		if (UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance())
		{
			AnimInst->Montage_Stop(0.15f);
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
