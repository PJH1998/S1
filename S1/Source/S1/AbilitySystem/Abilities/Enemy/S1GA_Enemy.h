// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GA_Enemy.generated.h"

struct FS1MontageData;
struct FS1MontageSet;
class UAnimMontage;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class S1_API US1GA_Enemy : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GA_Enemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnInputReactivated() override;

public:
	const FS1MontageSet* GetMontage() const;
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	AnimDataTag;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	MontageTag;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveMontage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTagContainer	CooldownTags;
	
};
