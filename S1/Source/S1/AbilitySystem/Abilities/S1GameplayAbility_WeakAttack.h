// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GameplayAbility_WeakAttack.generated.h"

class US1PlayerAnimInstance;
struct FS1MontageData;

UCLASS()
class S1_API US1GameplayAbility_WeakAttack : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility_WeakAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInputReactivated() override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void TryAdvanceCombo();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	const FS1MontageData* GetMontageData() const;
	US1PlayerAnimInstance* GetPlayerAnimInstance() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag AnimDataTag;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag MontageTag;

	int32 CurrentSectionIndex = 0;
};
