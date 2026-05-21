// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GA_Boss000_Attack00.generated.h"

struct FS1MontageData;
struct FS1MontageSet;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class S1_API US1GA_Boss000_Attack00 : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GA_Boss000_Attack00(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInputReactivated() override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	const FS1MontageSet* GetMontage() const;
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	AnimDataTag;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	MontageTag;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveMontage;
};
