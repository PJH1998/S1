// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "S1AbilityTask_RotateToCamera.generated.h"

UCLASS()
class S1_API US1AbilityTask_RotateToCamera : public UAbilityTask
{
	GENERATED_BODY()

public:
	// 공격 입력 시점의 카메라 Yaw를 목표로 회전, 도달 시 자동 종료
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static US1AbilityTask_RotateToCamera* RotateToCamera(UGameplayAbility* OwningAbility, float RotationSpeed = 720.f);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	float RotationSpeed;
	float TargetYaw;
};
