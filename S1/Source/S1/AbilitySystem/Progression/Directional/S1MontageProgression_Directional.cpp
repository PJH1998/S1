// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Directional/S1MontageProgression_Directional.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void US1MontageProgression_Directional::OnActivated()
{
	if (false == GA.IsValid())
	{
		return;
	}

	TObjectPtr<UAnimMontage>* Found = DirectionalMontages.Find(PendingDirection);
	if (nullptr == Found || false == IsValid(*Found))
	{
		GA->RequestEndAbility(true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task = GA->PlayAbilityMontageTask(*Found);
	if (nullptr == Task)
	{
		GA->RequestEndAbility(true);
		return;
	}

	// 네 개를 다 붙여야 중단 시 어빌리티가 끝난다 — OnCompleted만 걸면 인터럽트에서 멈춘다.
	// OnCompleted/OnBlendOut이 연달아 올 수 있지만 EndAbility가 IsEndAbilityValid로 걸러낸다
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageAborted);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageAborted);

	// 재생은 여기서 시작된다. 몽타주가 null이거나 재생 실패면 이 안에서 OnCancelled가 즉시 발화한다
	Task->ReadyForActivation();
}

void US1MontageProgression_Directional::OnMontageFinished()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(false);
	}
}

void US1MontageProgression_Directional::OnMontageAborted()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(true);
	}
}
