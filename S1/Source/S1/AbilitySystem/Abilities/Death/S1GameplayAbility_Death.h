// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hit/Weak/S1GameplayAbility_Hit_Weak.h"
#include "S1GameplayAbility_Death.generated.h"

// 사망 처리 = Hit_Weak(넉백/방향 없는 제자리 리액션 로직 그대로 재사용) + 콜리전 비활성화.
// 애니메이션 제약상 피격 타입(Weak/Strong/ToAir)과 무관하게 사망은 이 GA 하나로 통일 —
// HitType.Death GameplayEvent 하나로만 트리거된다(원래 피격 타입 정보는 버려짐).
// MontageProgression(BP에서 배정)에 Start/Loop만 채우고 End는 비워두면 Loop가 영원히 반복.
// "리스폰 가능" 신호는 몽타주에 배치한 US1AnimNotify_GrantLooseGameplayTag(State.Dead.CanRespawn)가 직접 처리.
UCLASS()
class S1_API US1GameplayAbility_Death : public US1GameplayAbility_Hit_Weak
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
