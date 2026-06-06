// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Dash/S1GameplayAbility_Dash.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1GameplayAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(MontageProgression))
	{
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}

	const FVector DashDir = ComputeDashDirection();

	UAbilityTask_ApplyRootMotionConstantForce* Task = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		DashDir,
		DashSpeed,
		DashDuration,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		false // bEnableGravity: 수평 고정
	);

	Task->OnFinish.AddDynamic(this, &ThisClass::OnDashFinished);
	Task->ReadyForActivation();
}

void US1GameplayAbility_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_Dash::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (AttackAbilityGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(AttackAbilityGroupTags))
	{
		return false;
	}

	// 대쉬 중 공격 입력 — 윈도우 조건 ���이 즉시 전환
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	return true;
}

void US1GameplayAbility_Dash::OnDashFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FVector US1GameplayAbility_Dash::ComputeDashDirection() const
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
