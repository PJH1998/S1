// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "S1GameplayAbility_Evasion.generated.h"

// 회피 계열 GA 공통 베이스 (Dash, Dodge)
// - CMC OrientRotationToMovement 저장/복원
// - 카메라 기준 입력 방향 계산
// - MoveBegin/End 이벤트 기반 이동 처리 (ConstantForce + GravityScale 제어)
UCLASS(Abstract)
class S1_API US1GameplayAbility_Evasion : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Evasion();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 카메라 기준 입력 방향 반환 (입력 없으면 액터 전방)
	FVector ComputeInputDirection() const;

	// 활성화 시점 루트모션 방향 — Evasion은 입력 방향(CapturedMoveDirection) 기준
	// (베이스는 컨트롤회전 방향 반환 → 4방향 회피와 불일치하므로 override 필수)
	virtual FVector GetCapturedMoveDirection() const override;

	// 몽타주 재생 직후 — LooseGameplayTag 노티파이 스캔 → 서버 무적 윈도우 타이머 스케줄
	virtual void OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate) override;


	// ActivateAbility 시 캡처한 이동 방향 (Dodge에서 회전 대상으로도 활용)
	FVector CapturedMoveDirection = FVector::ZeroVector;

private:
	// 서버 권위에서만 — 몽타주의 LooseGameplayTag(State.Invincible) 윈도우를 타이머로 스케줄
	void ScheduleInvincibilityWindows(UAnimMontage* Montage, float Rate);
	void ClearInvincibilityTimers();

	TArray<FTimerHandle> InvincibilityTimers;
};
