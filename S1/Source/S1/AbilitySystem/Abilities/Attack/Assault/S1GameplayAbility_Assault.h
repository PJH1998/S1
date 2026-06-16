// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Assault.generated.h"

// 카메라 방향 대쉬 공격 — 이동은 몽타주 NotifyState(MoveEvent)가 제어
// MoveEnd 이벤트 수신 시 공중/지상 판단 → Progression에 다음 섹션 직접 전달
UCLASS()
class S1_API US1GameplayAbility_Assault : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	virtual void OnMoveBeginReceived(const FGameplayEventData* Payload) override;
	virtual void OnMoveEndReceived(const FGameplayEventData* Payload) override;

private:
	// 공중/지상 판단 후 Progression의 EndMontages에서 조회할 키 태그
	UPROPERTY(EditDefaultsOnly, Category = "Assault|Branch")
	FGameplayTag AirEndTag;

	UPROPERTY(EditDefaultsOnly, Category = "Assault|Branch")
	FGameplayTag GroundEndTag;

	// 이동 구간 히트 강도 태그 — PostGE에서 피격 반응 선택에 사용
	UPROPERTY(EditDefaultsOnly, Category = "Assault")
	FGameplayTag AssaultHitStrengthTag;

	// 이동 구간 공격 배율
	UPROPERTY(EditDefaultsOnly, Category = "Assault")
	float AssaultAtkScale = 1.0f;

	// 버튼 누른 순간(ActivateAbility, Super 전)에 캡처 — RotateToCamera 회전 전 방향 보존
	FVector CapturedAssaultDirection = FVector::ZeroVector;

	// PlayAnimMontage(EndMontage)가 BeginMontage NotifyEnd를 동기 호출 시 중복 발화 방지
	bool bBranchRequested = false;
};
