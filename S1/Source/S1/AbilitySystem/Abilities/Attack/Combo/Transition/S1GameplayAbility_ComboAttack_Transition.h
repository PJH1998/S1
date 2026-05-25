// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/Combo/S1GameplayAbility_ComboAttack.h"
#include "S1GameplayAbility_ComboAttack_Transition.generated.h"

// 마지막 콤보에서 상태 전환(Air/Ground 등)이 필요한 콤보 어택
// TransitionStateTag 에디터 설정으로 전환 조건 지정
UCLASS()
class S1_API US1GameplayAbility_ComboAttack_Transition : public US1GameplayAbility_ComboAttack
{
	GENERATED_BODY()

public:
	// 마지막 콤보 + TransitionStateTag 활성 시 → 동일 AbilityTag로 재발동 (전환 GA 활성화)
	virtual bool OnInputReactivated() override;

protected:
	// CanNextAttackTag OR TransitionStateTag 중 하나라도 활성이면 윈도우 오픈
	virtual bool CanNextAction(UAbilitySystemComponent* ASC) const override;

private:
	// 전환 조건 태그 (ex. State.Air, State.Ground) — 에디터에서 설정
	UPROPERTY(EditDefaultsOnly, Category = "Transition")
	FGameplayTag TransitionStateTag;
};
