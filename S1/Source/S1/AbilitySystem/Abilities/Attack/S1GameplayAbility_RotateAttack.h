// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_RotateAttack.generated.h"

class US1AbilityTask_RotateToCamera;

UCLASS()
class S1_API US1GameplayAbility_RotateAttack : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

protected:
	virtual void OnBeforePlayMontage() override;

private:
	void StartRotateToCamera();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float RotationSpeed = 720.f;

	UPROPERTY()
	TObjectPtr<US1AbilityTask_RotateToCamera> RotateTask;
};
