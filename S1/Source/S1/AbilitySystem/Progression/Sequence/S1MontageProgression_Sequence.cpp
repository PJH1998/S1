// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Sequence/S1MontageProgression_Sequence.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Pawn.h"

void US1MontageProgression_Sequence::OnActivated()
{
	ReleaseMontageTask();

	CurrentComboIndex = 0;
	PlayComboAtIndex();
}

void US1MontageProgression_Sequence::OnDeactivated()
{
	ReleaseMontageTask();

	CurrentComboIndex = 0;
}

bool US1MontageProgression_Sequence::OnInputReactivated()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	return TryAdvanceCombo();
}

bool US1MontageProgression_Sequence::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty() || false == TargetAbilityTags.HasAny(CrossComboGroupTags))
	{
		return false;
	}

	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	GA->RequestEndAbility(false);
	return true;
}

void US1MontageProgression_Sequence::PlayComboAtIndex()
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

	// SectionNames가 비어 있으면 NAME_None → 몽타주 처음부터 전체를 1타로 재생
	const FName SectionName = SectionNames.IsValidIndex(CurrentComboIndex) ? SectionNames[CurrentComboIndex] : NAME_None;

	// 1타만 몽타주를 재생한다 — 2타+는 재생 중인 몽타주 안에서 섹션 점프이므로 태스크가 그대로 살아 있다
	if (0 == CurrentComboIndex)
	{
		StartMontageTask(SectionMontage, SectionName);
		return;
	}

	// 섹션 점프는 하드컷 — 몽타주 분리와 달리 Blend In/Out이 안 붙어서 포즈를 직접 이어줘야 한다
	if (SectionInertializationBlendTime > 0.f)
	{
		if (US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression())
		{
			AnimInst->RequestInertialization(SectionInertializationBlendTime);
		}
	}

	GA->JumpToMontageSection(SectionMontage, SectionName);
}

int32 US1MontageProgression_Sequence::GetComboCount() const
{
	// SectionNames가 비면 1타짜리
	return FMath::Max(SectionNames.Num(), 1);
}

bool US1MontageProgression_Sequence::TryAdvanceCombo()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();

	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	const int32 NextIndex = CurrentComboIndex + 1;
	if (NextIndex >= GetComboCount())
	{
		// 마지막 콤보 → NextAttackAbilityTag 있으면 해당 어빌리티로 전이
		if (NextAttackAbilityTag.IsValid())
		{
			GA->RequestEndAbility(false);
			GA->RequestActivateAbilityByTag(FGameplayTagContainer(NextAttackAbilityTag));
			return true;
		}

		// 자동 루프백 안 함 — 몽타주 종료 시 어빌리티 종료 → 재입력은 새 활성화(예측)로 콤보1
		// (LocalPredicted에서 루프백 재활성은 예측+서버RPC 이중/순서 문제를 일으킴)
		return false;
	}

	CurrentComboIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(CanNextAttackTag);
	PlayComboAtIndex();
	GA->OnProgressionMontageStarted();

	return true;
}

bool US1MontageProgression_Sequence::CanNextAction(UAbilitySystemComponent* ASC) const
{
	// 서버가 원격 클라(autonomous proxy)를 미러링하는 경우 — 콤보 윈도우 검사는 클라가 이미 수행 후
	// 예측키와 함께 RPC를 보냄 → 신뢰하고 통과. 서버 몽타주의 윈도우 태그는 RTT 스큐 + 대기 루프로
	// 매 프레임 토글되어 불안정 → 재검사 시 정상 입력도 우연히 OFF 순간에 거부됨 (콤보 desync 원인)
	if (GA.IsValid())
	{
		if (const APawn* Pawn = Cast<APawn>(GA->GetAvatarActorFromActorInfo()))
		{
			if (Pawn->HasAuthority() && false == Pawn->IsLocallyControlled())
			{
				return true;
			}
		}
	}

	return ASC->HasMatchingGameplayTag(CanNextAttackTag);
}

void US1MontageProgression_Sequence::StartMontageTask(UAnimMontage* Montage, FName SectionName)
{
	ReleaseMontageTask();

	ActiveMontageTask = GA->PlayAbilityMontageTask(Montage, SectionName);
	if (false == ::IsValid(ActiveMontageTask))
	{
		GA->RequestEndAbility(true);
		return;
	}

	// 네 개를 다 붙여야 중단 시 어빌리티가 끝난다 — OnCompleted만 걸면 인터럽트에서 멈춘다.
	// OnCompleted/OnBlendOut이 연달아 올 수 있지만 EndAbility가 IsEndAbilityValid로 걸러낸다
	ActiveMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageAborted);
	ActiveMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageAborted);

	// 재생은 여기서 시작된다. 몽타주가 null이거나 재생 실패면 이 안에서 OnCancelled가 즉시 발화한다
	ActiveMontageTask->ReadyForActivation();
}

void US1MontageProgression_Sequence::ReleaseMontageTask()
{
	if (false == ::IsValid(ActiveMontageTask))
	{
		ActiveMontageTask = nullptr;
		return;
	}

	// 바인딩을 먼저 끊는다 — 다음 타의 몽타주가 이전 몽타주를 인터럽트하면서 이전 태스크의
	// OnInterrupted가 발화해 콤보 도중 어빌리티를 끝내는 것을 막는다
	ActiveMontageTask->OnCompleted.RemoveAll(this);
	ActiveMontageTask->OnBlendOut.RemoveAll(this);
	ActiveMontageTask->OnInterrupted.RemoveAll(this);
	ActiveMontageTask->OnCancelled.RemoveAll(this);
	ActiveMontageTask->EndTask();
	ActiveMontageTask = nullptr;
}

void US1MontageProgression_Sequence::OnMontageFinished()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(false);
	}
}

void US1MontageProgression_Sequence::OnMontageAborted()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(true);
	}
}