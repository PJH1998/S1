// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "S1GA_Enemy001_Attack01.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GA_Enemy001_Attack01 : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Enemy001_Attack01(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName StartSection = FName("Start");

	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName LoopSection = FName("Loop");

	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName EndSection = FName("End");
};
