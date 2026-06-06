// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_LoopAttack_Dive.generated.h"

UCLASS()
class S1_API US1GameplayAbility_LoopAttack_Dive : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Loop 종료 직전 호출 — 중력 리셋 후 End 섹션 진입
	virtual void OnProgressionLoopEnded() override;

private:
	UFUNCTION()
	void OnDiveStart(FGameplayEventData Payload);

private:
	// 하강 시작 이벤트 태그 (AnimNotify에서 발생)
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	FGameplayTag DiveEventTag;

	// Start 구간 중력 (0 = 공중 정지)
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	float StartGravityScale = 0.f;

	// Loop 구간 중력
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	float DiveGravityScale = 3.f;
};
