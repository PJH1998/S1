// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Evasion/S1GameplayAbility_Evasion.h"
#include "S1GameplayAbility_Dodge.generated.h"

class US1AbilityTask_RotateToCamera;

// 구르기 회피 — 입력 방향 즉시 회전 + ConstantForce 이동 + 무적 구간 지원
// 이동 처리: Evasion 베이스 (MoveBeginEventTag/MoveEndEventTag BP에서 설정)
// 무적 구간: 몽타주에 AnimNotifyState_LooseGameplayTag (Tag = State.Invincible) 배치
UCLASS()
class S1_API US1GameplayAbility_Dodge : public US1GameplayAbility_Evasion
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 입력 방향으로의 회전 보간 속도 (deg/s) — 짧은 구르기 시작 구간에 맞게 높게 설정
	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Rotation")
	float RotationSpeed = 720.f;

	UPROPERTY()
	TObjectPtr<US1AbilityTask_RotateToCamera> RotateTask;
};
