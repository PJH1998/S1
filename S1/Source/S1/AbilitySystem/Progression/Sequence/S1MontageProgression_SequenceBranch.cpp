// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Sequence/S1MontageProgression_SequenceBranch.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/S1AnimInstance.h"

void US1MontageProgression_SequenceBranch::OnActivated()
{
	if (false == GA.IsValid())
	{
		return;
	}

	if (false == ::IsValid(SectionMontage))
	{
		GA->RequestEndAbility(true);
		return;
	}

	ReleaseMontageTask();

	ActiveMontageTask = GA->PlayAbilityMontageTask(SectionMontage, StartSectionName);
	if (false == ::IsValid(ActiveMontageTask))
	{
		GA->RequestEndAbility(true);
		return;
	}

	// 네 개를 다 붙여야 중단 시 어빌리티가 끝난다 — OnCompleted만 걸면 인터럽트에서 멈춘다
	ActiveMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageAborted);
	ActiveMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageAborted);
	ActiveMontageTask->ReadyForActivation();

	// 분기가 늦게 와도 몽타주가 Start 경계에서 끝나지 않게 한다 (FallbackEndSectionName 주석 참조)
	GA->SetMontageNextSection(StartSectionName, FallbackEndSectionName);
}

void US1MontageProgression_SequenceBranch::OnDeactivated()
{
	ReleaseMontageTask();
}

void US1MontageProgression_SequenceBranch::OnBranchRequested(FGameplayTag InKey)
{
	if (false == GA.IsValid())
	{
		return;
	}

	const FName* Found = EndSectionNames.Find(InKey);
	if (nullptr == Found || Found->IsNone())
	{
		GA->RequestEndAbility(true);
		return;
	}

	// 섹션 점프는 하드컷이라 포즈를 직접 이어준다
	if (BranchInertializationBlendTime > 0.f)
	{
		if (US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression())
		{
			AnimInst->RequestInertialization(BranchInertializationBlendTime);
		}
	}

	// End 섹션이 끝나면 몽타주도 끝나고 태스크의 OnCompleted가 어빌리티를 종료한다
	GA->SetMontageNextSection(*Found, NAME_None);
	GA->JumpToMontageSection(SectionMontage, *Found);
}

void US1MontageProgression_SequenceBranch::ReleaseMontageTask()
{
	if (false == ::IsValid(ActiveMontageTask))
	{
		ActiveMontageTask = nullptr;
		return;
	}

	ActiveMontageTask->OnCompleted.RemoveAll(this);
	ActiveMontageTask->OnBlendOut.RemoveAll(this);
	ActiveMontageTask->OnInterrupted.RemoveAll(this);
	ActiveMontageTask->OnCancelled.RemoveAll(this);
	ActiveMontageTask->EndTask();
	ActiveMontageTask = nullptr;
}

void US1MontageProgression_SequenceBranch::OnMontageFinished()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(false);
	}
}

void US1MontageProgression_SequenceBranch::OnMontageAborted()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(true);
	}
}