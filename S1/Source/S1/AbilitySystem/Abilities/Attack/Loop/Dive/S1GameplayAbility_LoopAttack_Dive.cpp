// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Loop/Dive/S1GameplayAbility_LoopAttack_Dive.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void US1GameplayAbility_LoopAttack_Dive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Attack: CommitAbility + UsedTag
	// LoopAttack: Velocity=0 + PlayMontage(StartSection) + Loop루프 + LoopEndEventTag 리스너
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Start 구간: 공중 정지
	SetGravityScale(StartGravityScale);

	// 하강 시작 이벤트 등록 (AnimNotify에서 발생)
	if (DiveEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* DiveTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DiveEventTag);
		DiveTask->EventReceived.AddDynamic(this, &ThisClass::OnDiveStart);
		DiveTask->ReadyForActivation();
	}
}

void US1GameplayAbility_LoopAttack_Dive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetGravityScale();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_LoopAttack_Dive::OnLoopEndEvent(FGameplayEventData Payload)
{
	ResetGravityScale();

	Super::OnLoopEndEvent(Payload); // Loop → End 섹션 전환
}

void US1GameplayAbility_LoopAttack_Dive::OnDiveStart(FGameplayEventData Payload)
{
	SetGravityScale(DiveGravityScale);
}
