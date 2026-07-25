// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Sequence/S1MontageProgression_Sequence.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"

void US1MontageProgression_Sequence::OnActivated()
{
	CurrentComboIndex = 0;
	PlayMontageAtIndex();
}

void US1MontageProgression_Sequence::OnDeactivated()
{
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

void US1MontageProgression_Sequence::PlayMontageAtIndex()
{
	if (false == GA.IsValid())
	{
		return;
	}

	if (false == ComboMontages.IsValidIndex(CurrentComboIndex) || false == IsValid(ComboMontages[CurrentComboIndex]))
	{
		GA->RequestEndAbility(true);
		return;
	}

	UAnimMontage* CurrentMontage = ComboMontages[CurrentComboIndex];

	// ASC 경유 재생 → 클라 복제
	const float PlayDuration = GA->PlayAbilityMontage(CurrentMontage);

	// [AirCombo] 진단 — 조인 클라(AUTO)에서 duration<=0이면 몽타주가 실제로 안 돎(예측 재생 실패) → GA 조기 종료 원인
	{
		const AActor* Av = GA->GetAvatarActorFromActorInfo();
		LOG(TEXT("[AirCombo][%s] PlayMontageAtIndex idx=%d montage=%s duration=%.2f"),
			Av ? (Av->GetLocalRole() == ROLE_Authority ? TEXT("AUTH") : Av->GetLocalRole() == ROLE_AutonomousProxy ? TEXT("AUTO") : TEXT("SIM")) : TEXT("?"),
			CurrentComboIndex, *CurrentMontage->GetName(), PlayDuration);
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, CurrentMontage);
	}
}

bool US1MontageProgression_Sequence::TryAdvanceCombo()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();

	// [AirCombo] 진단 — 조인 클라(AUTO)에서 재입력 시 canNextTag=0이면 콤보 윈도우가 안 열려 진행 거부됨
	{
		const AActor* Av = GA->GetAvatarActorFromActorInfo();
		LOG(TEXT("[AirCombo][%s] TryAdvanceCombo idx=%d canNextTag=%d air=%d"),
			Av ? (Av->GetLocalRole() == ROLE_Authority ? TEXT("AUTH") : Av->GetLocalRole() == ROLE_AutonomousProxy ? TEXT("AUTO") : TEXT("SIM")) : TEXT("?"),
			CurrentComboIndex,
			(IsValid(ASC) && ASC->HasMatchingGameplayTag(CanNextAttackTag)) ? 1 : 0,
			(IsValid(ASC) && ASC->HasMatchingGameplayTag(S1StateTags::State_Air)) ? 1 : 0);
	}

	if (false == IsValid(ASC) || false == CanNextAction(ASC))
	{
		return false;
	}

	const int32 NextIndex = CurrentComboIndex + 1;
	if (false == ComboMontages.IsValidIndex(NextIndex))
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
	PlayMontageAtIndex();
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

void US1MontageProgression_Sequence::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (false == ComboMontages.IsValidIndex(CurrentComboIndex) || Montage != ComboMontages[CurrentComboIndex])
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
