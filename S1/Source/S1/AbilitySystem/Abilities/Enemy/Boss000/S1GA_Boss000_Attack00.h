// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "S1GA_Boss000_Attack00.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GA_Boss000_Attack00 : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_Attack00(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool OnInputReactivated() override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
