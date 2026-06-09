// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Evasion/S1GameplayAbility_Evasion.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1GameplayAbility_Evasion::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Super 전 — 몽타주 시작 전 방향 캡처 + 회전 제어권 확보
	CapturedMoveDirection = ComputeInputDirection();

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		bCachedOrientToMovement = CMC->bOrientRotationToMovement;
		CMC->bOrientRotationToMovement = false;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (MoveBeginEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* BeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveBeginEventTag);
		BeginTask->EventReceived.AddDynamic(this, &ThisClass::OnMoveBeginReceived);
		BeginTask->ReadyForActivation();
	}

	if (MoveEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* EndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveEndEventTag);
		EndTask->EventReceived.AddDynamic(this, &ThisClass::OnMoveEndReceived);
		EndTask->ReadyForActivation();
	}
}

void US1GameplayAbility_Evasion::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 안전망 — MoveEnd 이벤트 없이 종료된 경우 대비
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		CMC->GravityScale = 1.f;
		CMC->bOrientRotationToMovement = bCachedOrientToMovement;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Evasion::OnMoveBeginReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		return;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		CapturedMoveDirection,
		MoveSpeed,
		9999.f,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		false
	);
	MoveTask->ReadyForActivation();

	if (bControlGravity)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->GetCharacterMovement()->GravityScale = 0.f;
		}
	}
}

void US1GameplayAbility_Evasion::OnMoveEndReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (bControlGravity)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
			Character->LaunchCharacter(CapturedMoveDirection * CMC->MaxWalkSpeed, true, false);
			CMC->GravityScale = 1.f;
		}
	}
}

FVector US1GameplayAbility_Evasion::ComputeInputDirection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return FVector::ForwardVector;
	}

	FVector InputDir = Character->GetLastMovementInputVector();
	if (InputDir.IsNearlyZero())
	{
		InputDir = Character->GetActorForwardVector();
	}

	InputDir.Z = 0.f;
	return InputDir.GetSafeNormal();
}

