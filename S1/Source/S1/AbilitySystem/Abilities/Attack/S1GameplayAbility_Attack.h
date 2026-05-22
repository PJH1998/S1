// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GameplayAbility_Attack.generated.h"

struct FS1MontageData;
struct FS1MontageSet;
class US1AbilityTask_RotateToCamera;

UCLASS()
class S1_API US1GameplayAbility_Attack : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	const FS1MontageData* GetMontageData() const;
	virtual const FS1MontageSet* GetCurrentMontageSet() const; // 기본: index 0

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag AnimDataTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag UsedTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	bool bRotateToCamera = false;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (EditCondition = "bRotateToCamera"))
	float RotationSpeed = 720.f;

private:
	void StartRotateToCamera();

	UPROPERTY()
	TObjectPtr<US1AbilityTask_RotateToCamera> RotateTask;
};
