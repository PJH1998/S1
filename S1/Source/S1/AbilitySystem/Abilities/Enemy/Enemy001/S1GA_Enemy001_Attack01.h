// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "TimerManager.h"
#include "S1GA_Enemy001_Attack01.generated.h"

class UAnimMontage;

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
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnCheckDistanceEvent(FGameplayEventData Payload);

	void UpdateNextSectionByDistance();
	float GetDistanceToTarget() const;
	AActor* GetTargetActor() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName StartSection = FName("Start");

	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName LoopSection = FName("Loop");

	UPROPERTY(EditDefaultsOnly, Category = "Attack01|Section")
	FName EndSection = FName("End");

	UPROPERTY(EditDefaultsOnly, Category = "Attack01")
	FGameplayTag CheckDistanceEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack01")
	float LoopDistance = 500.f;
};
