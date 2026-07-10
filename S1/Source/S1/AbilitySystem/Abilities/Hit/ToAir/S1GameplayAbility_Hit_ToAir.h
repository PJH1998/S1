// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "S1GameplayAbility_Hit_ToAir.generated.h"

// 공중 띄우기 피격 — 넉백(수평, Strong과 동일 계산) + 상승(수직) 후 Start→Loop→End 흐름은
// MontageProgression에 배정한 US1MontageProgression_Loop가 전부 처리:
//   - StartMontage에 US1AnimNotifyState_HitLaunch(bMaintainVelocityOnFinish=true) 배치 — 순간 임팩트만 주고
//     이후 CMC 중력에 맡겨 자연스러운 포물선 상승/낙하
//   - LoopEndEventTag = Event.Landed(AS1Player::Landed()가 보내는 기존 이벤트), bImmediateExitOnLoopEnd = true
//     → 착지 즉시 EndMontage로 전환 (EndMontage에 착지+기상을 한 클립으로 구성)
UCLASS()
class S1_API US1GameplayAbility_Hit_ToAir : public US1GameplayAbility_Hit
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Hit_ToAir(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 넉백 수평 속도(cm/s) — 공격 쪽에서 전달받지 않는 고정값 (ComputeKnockback의 수평 성분)
	UPROPERTY(EditDefaultsOnly, Category = "Hit|ToAir")
	float LaunchSpeed = 600.f;

	// 넉백에 더해지는 상승 속도(cm/s) — ComputeKnockback은 수평 성분만 계산하므로 Z는 여기서 추가
	UPROPERTY(EditDefaultsOnly, Category = "Hit|ToAir")
	float LaunchUpSpeed = 600.f;
};
