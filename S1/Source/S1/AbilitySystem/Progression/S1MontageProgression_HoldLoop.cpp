// Fill out your copyright notice in the Description page of Settings.

#include "AbilitySystem/Progression/S1MontageProgression_HoldLoop.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/S1AnimInstance.h"
#include "Data/S1AnimData.h"

void US1MontageProgression_HoldLoop::OnActivated()
{
	LoopCount = 0;
	bReleased = false;
	bBranchRequested = false;

	Super::OnActivated();

	// Super 내부 실패 시 RequestEndAbility 호출됨 — 종료된 GA에 Task 등록 방지
	if (false == GA.IsValid() || false == GA->IsActive())
	{
		return;
	}

	if (LoopCycleEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(GA.Get(), LoopCycleEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnLoopCycleEventReceived);
		Task->ReadyForActivation();
	}
}

void US1MontageProgression_HoldLoop::OnDeactivated()
{
	LoopCount = 0;
	bReleased = false;
	bBranchRequested = false;
}

void US1MontageProgression_HoldLoop::OnInputReleased()
{
	bReleased = true;

	// Loop 재생 중이면 즉시 링크 변경 — Start 중이면 첫 사이클 이벤트에서 처리 (최소 1회 보장)
	BranchToEnd();
}

void US1MontageProgression_HoldLoop::OnLoopCycleEventReceived(FGameplayEventData Payload)
{
	if (bBranchRequested)
	{
		return;
	}

	++LoopCount;
	if (bReleased || LoopCount >= MaxLoopCount)
	{
		BranchToEnd();
	}
}

void US1MontageProgression_HoldLoop::BranchToEnd()
{
	if (bBranchRequested)
	{
		return;
	}

	if (false == GA.IsValid())
	{
		return;
	}

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (false == IsValid(AnimInst))
	{
		return;
	}

	// Loop 재생 중에만 분기 — Start 중이면 무시 (OnLoopCycleEventReceived가 처리)
	const FName CurrentSection = AnimInst->Montage_GetCurrentSection(MontageSet->Montage);
	if (CurrentSection != LoopSection)
	{
		return;
	}

	// 현재 사이클은 끝까지 재생 후 End로
	AnimInst->Montage_SetNextSection(LoopSection, EndSection, MontageSet->Montage);
	bBranchRequested = true;
}
