// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/S1MontageProgression_SectionBranch.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Animation/S1AnimInstance.h"
#include "Data/S1AnimData.h"
#include "GameFramework/Character.h"

void US1MontageProgression_SectionBranch::OnActivated()
{
	bBranched = false;

	if (false == GA.IsValid())
	{
		return;
	}

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		GA->RequestEndAbility(true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	const float Duration = Character->PlayAnimMontage(MontageSet->Montage, 1.f, BeginSection);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
	}
}

void US1MontageProgression_SectionBranch::OnDeactivated()
{
	bBranched = false;
}

void US1MontageProgression_SectionBranch::OnBranchRequested(FName SectionName)
{
	if (false == GA.IsValid())
	{
		return;
	}

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (false == IsValid(AnimInst))
	{
		GA->RequestEndAbility(true);
		return;
	}

	bBranched = true;

	if (AnimInst->Montage_IsPlaying(MontageSet->Montage))
	{
		// Begin 아직 재생 중 → 현재 섹션 끝나면 자연스럽게 연결
		AnimInst->Montage_SetNextSection(BeginSection, SectionName, MontageSet->Montage);
	}
	else
	{
		// Begin 이미 종료 → 해당 섹션 직접 재시작
		ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
		if (false == IsValid(Character))
		{
			GA->RequestEndAbility(true);
			return;
		}

		const float Duration = Character->PlayAnimMontage(MontageSet->Montage, 1.f, SectionName);
		if (Duration <= 0.f)
		{
			GA->RequestEndAbility(true);
			return;
		}

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
	}
}

void US1MontageProgression_SectionBranch::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet || Montage != MontageSet->Montage)
	{
		return;
	}

	if (false == bBranched)
	{
		// Begin이 분기 요청 전에 종료 — 무시 (OnBranchRequested가 곧 호출됨)
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
