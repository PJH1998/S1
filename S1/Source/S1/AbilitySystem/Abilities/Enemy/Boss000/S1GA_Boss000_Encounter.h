// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "S1GA_Boss000_Encounter.generated.h"

/**
 * 보스 최초 조우 인트로 어빌리티. 첫 타겟 인식 시 BT가 1회 발동하며, 인트로 몽타주를 재생한다.
 */
UCLASS()
class S1_API US1GA_Boss000_Encounter : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_Encounter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};