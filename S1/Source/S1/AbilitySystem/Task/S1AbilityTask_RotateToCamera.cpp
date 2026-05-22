// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

US1AbilityTask_RotateToCamera* US1AbilityTask_RotateToCamera::RotateToCamera(UGameplayAbility* OwningAbility, float RotationSpeed)
{
	US1AbilityTask_RotateToCamera* Task = NewAbilityTask<US1AbilityTask_RotateToCamera>(OwningAbility);
	Task->RotationSpeed = RotationSpeed;
	Task->bTickingTask = true;
	return Task;
}

void US1AbilityTask_RotateToCamera::UpdateTargetYaw(float NewYaw)
{
	TargetYaw = NewYaw;
}

void US1AbilityTask_RotateToCamera::Activate()
{
	Super::Activate();

	// 입력 받은 이 순간의 카메라 Yaw 고정
	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!PC)
	{
		EndTask();
		return;
	}

	TargetYaw = PC->GetControlRotation().Yaw;
}

void US1AbilityTask_RotateToCamera::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (!Character)
	{
		EndTask();
		return;
	}

	const FRotator Current = Character->GetActorRotation();
	const FRotator Target = FRotator(0.f, TargetYaw, 0.f);
	const FRotator Next = FMath::RInterpConstantTo(Current, Target, DeltaTime, RotationSpeed);
	Character->SetActorRotation(Next);

	// 자기 종료 없음 — Ability 종료 시 GAS가 자동으로 태스크를 정리함
}
