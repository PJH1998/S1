// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/S1MontageProgression_Sequence.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "Character/S1Character.h"
#include "Data/S1AnimData.h"

void US1MontageProgression_Sequence::OnActivated()
{
	CurrentSectionIndex = 0;
	PlayMontageAtIndex();
}

void US1MontageProgression_Sequence::OnDeactivated()
{
	CurrentSectionIndex = 0;
}

bool US1MontageProgression_Sequence::OnInputReactivated()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();

	// Transition 우선: TransitionStateTag 활성 시 동일 AbilityTag 재발동
	if (TransitionStateTag.IsValid() && IsValid(ASC) && ASC->HasMatchingGameplayTag(TransitionStateTag))
	{
		GA->RequestReactivateSelf();
		return true;
	}

	return TryAdvanceCombo();
}

bool US1MontageProgression_Sequence::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(CrossComboGroupTags))
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

const FS1MontageSet* US1MontageProgression_Sequence::GetCurrentMontageSet() const
{
	if (false == GA.IsValid())
	{
		return nullptr;
	}

	const FS1MontageData* Data = GA->GetMontageData();
	if (nullptr == Data || false == Data->MontageSets.IsValidIndex(CurrentSectionIndex))
	{
		return nullptr;
	}

	return &Data->MontageSets[CurrentSectionIndex];
}

void US1MontageProgression_Sequence::PlayMontageAtIndex()
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

	AS1Character* Character = Cast<AS1Character>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	Character->PlayAnimMontage(MontageSet->Montage, 1.f, OverrideStartSection);
	OverrideStartSection = NAME_None;

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, MontageSet->Montage);
	}
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

	const FS1MontageData* MontageData = GA->GetMontageData();
	if (nullptr == MontageData)
	{
		return false;
	}

	int32 NextIndex = CurrentSectionIndex + 1;
	if (false == MontageData->MontageSets.IsValidIndex(NextIndex))
	{
		// 마지막 콤보 → 무기별 NextAttackAbilityTag로 전이
		if (const FGameplayTag* WeaponTag = NextAttackAbilityTagByWeapon.Find(GA->GetEquippedWeaponType()))
		{
			GA->RequestEndAbility(false);
			GA->RequestActivateAbilityByTag(FGameplayTagContainer(*WeaponTag));
		}
		else
		{
			// 미설정 무기 → 자기 자신 재발동 (EndAbility 후 TryActivate — Blocked Tag 검사 통과 필요)
			GA->RequestReactivateSelf();
		}
		return true;
	}

	CurrentSectionIndex = NextIndex;
	ASC->RemoveLooseGameplayTag(CanNextAttackTag);
	PlayMontageAtIndex();
	GA->OnProgressionMontageStarted();

	return true;
}

bool US1MontageProgression_Sequence::CanNextAction(UAbilitySystemComponent* ASC) const
{
	if (ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return true;
	}

	return TransitionStateTag.IsValid() && ASC->HasMatchingGameplayTag(TransitionStateTag);
}

void US1MontageProgression_Sequence::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FS1MontageSet* CurrentSet = GetCurrentMontageSet();
	if (nullptr == CurrentSet || Montage != CurrentSet->Montage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}
