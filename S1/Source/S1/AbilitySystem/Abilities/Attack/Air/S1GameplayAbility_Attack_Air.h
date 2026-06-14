// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_Attack_Air.generated.h"

UCLASS()
class S1_API US1GameplayAbility_Attack_Air : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AirAttack")
	float AirGravityScale = 0.f;
};
