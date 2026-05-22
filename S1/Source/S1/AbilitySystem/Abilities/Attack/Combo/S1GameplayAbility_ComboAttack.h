// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_ComboAttack.generated.h"

UCLASS()
class S1_API US1GameplayAbility_ComboAttack : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

public:
	virtual void OnInputReactivated() override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual const FS1MontageSet* GetCurrentMontageSet() const override; // CurrentSectionIndex 사용

private:
	void PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo);
	void TryAdvanceCombo();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FGameplayTag NextAttackAbilityTag;

	int32 CurrentSectionIndex = 0;
};
