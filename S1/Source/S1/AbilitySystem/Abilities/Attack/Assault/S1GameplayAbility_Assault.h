// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Assault.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;

// 카메라 방향 대쉬 공격 — 이동은 몽타주 NotifyState(MoveEvent)가 제어
// MoveEnd 이벤트 수신 시 공중/지상 판단 → Progression에 다음 섹션 직접 전달
UCLASS()
class S1_API US1GameplayAbility_Assault : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnMoveBeginReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnMoveEndReceived(FGameplayEventData Payload);

	FVector ComputeAssaultDirection() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Assault")
	float AssaultSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Assault")
	FGameplayTag MoveBeginEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Assault")
	FGameplayTag MoveEndEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Assault|Section")
	FName AirSection = FName("End_Air");

	UPROPERTY(EditDefaultsOnly, Category = "Assault|Section")
	FName GroundSection = FName("End_Ground");

	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MoveTask;
};
