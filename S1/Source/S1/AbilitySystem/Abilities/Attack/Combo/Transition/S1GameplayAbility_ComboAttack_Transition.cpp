// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Combo/Transition/S1GameplayAbility_ComboAttack_Transition.h"
#include "AbilitySystemComponent.h"

bool US1GameplayAbility_ComboAttack_Transition::OnInputReactivated()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->HasMatchingGameplayTag(TransitionStateTag))
	{
		// 전환 상태 → 동일 AbilityTag 재발동으로 전환 GA 활성화
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		if (Spec)
		{
			ASC->TryActivateAbilitiesByTag(Spec->Ability->AbilityTags);
		}
		return true;
	}

	// TransitionStateTag 없음 → 1-2타 일반 콤보 어드밴스
	return Super::OnInputReactivated();
}

bool US1GameplayAbility_ComboAttack_Transition::CanNextAction(UAbilitySystemComponent* ASC) const
{
	// 일반 콤보 윈도우(CanNextAttackTag) 또는 전환 상태 태그 중 하나면 오픈
	return Super::CanNextAction(ASC) || ASC->HasMatchingGameplayTag(TransitionStateTag);
}
