// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GameplayAbility_Attack.generated.h"

class US1PlayerAnimInstance;
struct FS1MontageData;
struct FS1MontageSet;

UCLASS()
class S1_API US1GameplayAbility_Attack : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInputReactivated() override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnBeforePlayMontage() {}

	US1PlayerAnimInstance* GetPlayerAnimInstance() const;

private:
	void PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo);
	void TryAdvanceCombo();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	const FS1MontageData* GetMontageData() const;
	const FS1MontageSet* GetCurrentMontageSet() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag AnimDataTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag MontageTag;

	int32 CurrentSectionIndex = 0;
};
