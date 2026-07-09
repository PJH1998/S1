// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "S1GameplayAbility_Hit_Weak.generated.h"

// 약한 피격 — 방향별 제자리 경직 몽타주 재생 (넉백 없음)
// MontageProgression에 US1MontageProgression_Directional을 배정하고 DirectionalMontages에 방향별 경직 몽타주를 등록해서 사용
UCLASS()
class S1_API US1GameplayAbility_Hit_Weak : public US1GameplayAbility_Hit
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Hit_Weak(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
