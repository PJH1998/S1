// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Dash.generated.h"

// 입력 방향 수평 대쉬 — 공격으로 파생 가능
UCLASS()
class S1_API US1GameplayAbility_Dash : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	virtual bool OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnDashFinished();

	FVector ComputeDashDirection() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.3f;

	// 대쉬 도중 이 태그 그룹의 Ability 입력 → 즉시 전환 허용 (윈도우 조건 없음)
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	FGameplayTagContainer AttackAbilityGroupTags;
};
