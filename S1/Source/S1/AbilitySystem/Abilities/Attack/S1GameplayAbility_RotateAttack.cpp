// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_RotateAttack.h"
#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"

void US1GameplayAbility_RotateAttack::OnBeforePlayMontage()
{
	StartRotateToCamera();
}

void US1GameplayAbility_RotateAttack::StartRotateToCamera()
{
	if (RotateTask)
	{
		RotateTask->EndTask();
	}

	RotateTask = US1AbilityTask_RotateToCamera::RotateToCamera(this, RotationSpeed);
	RotateTask->ReadyForActivation();
}
