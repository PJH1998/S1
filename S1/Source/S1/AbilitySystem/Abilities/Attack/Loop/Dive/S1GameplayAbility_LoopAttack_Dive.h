// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/Loop/S1GameplayAbility_LoopAttack.h"
#include "S1GameplayAbility_LoopAttack_Dive.generated.h"

UCLASS()
class S1_API US1GameplayAbility_LoopAttack_Dive : public US1GameplayAbility_LoopAttack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ResetGravityScale 후 Super (Loop → End 섹션 전환)
	virtual void OnLoopEndEvent(FGameplayEventData Payload) override;

private:
	UFUNCTION()
	void OnDiveStart(FGameplayEventData Payload); // SetGravityScale(DiveGravityScale)

private:
	// 몽타주 Start 구간 내 AnimNotify에서 발생하는 하강 시작 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	FGameplayTag DiveEventTag;

	// Start 구간 중력 (0 = 공중 정지)
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	float StartGravityScale = 0.f;

	// Loop 구간 중력 (값이 클수록 빠르게 하강)
	UPROPERTY(EditDefaultsOnly, Category = "Dive")
	float DiveGravityScale = 3.f;
};
