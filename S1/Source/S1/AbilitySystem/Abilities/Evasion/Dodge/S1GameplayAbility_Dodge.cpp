// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Evasion/Dodge/S1GameplayAbility_Dodge.h"
#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"

void US1GameplayAbility_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 방향 캡처(CapturedMoveDirection) + OrientRotation + MoveBegin/End 바인딩은 Evasion에서 처리
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 입력 방향으로 즉시 회전 시작
	// ReadyForActivation() 내부에서 카메라 Yaw를 캡처하지만 UpdateTargetYaw로 입력 방향으로 덮어씀
	RotateTask = US1AbilityTask_RotateToCamera::RotateToCamera(this, RotationSpeed);
	RotateTask->ReadyForActivation();
	RotateTask->UpdateTargetYaw(CapturedMoveDirection.Rotation().Yaw);

	if (IsValid(MontageProgression))
	{
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}
}
