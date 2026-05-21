// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_AirAttack.generated.h"

/**
 *
 */
UCLASS()
class S1_API US1GameplayAbility_AirAttack : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void SetGravityScale(float Scale) const;

	// 발동 시 ASC에 추가되는 "사용됨" 태그 — Activation Blocked Tags에 동일 태그 설정 시 착지 전 재발동 방지
	UPROPERTY(EditDefaultsOnly, Category = "AirAttack")
	FGameplayTag UsedTag;

	UPROPERTY(EditDefaultsOnly, Category = "AirAttack")
	float AirGravityScale = 0.f;
};
