// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Task/S1AbilityTask_DelayedRootMotionForce.h"
#include "AbilitySystem/RootMotion/S1RootMotionSource_DelayedConstantForce.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "S1LogChannels.h"

namespace
{
	// 엔진 ApplyRootMotionConstantForce와 동일한 우선순위
	constexpr uint16 DelayedForcePriority = 5;
}

US1AbilityTask_DelayedRootMotionForce* US1AbilityTask_DelayedRootMotionForce::ApplyDelayedRootMotionConstantForce(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	FVector WorldDirection,
	float Strength,
	float Duration,
	float WindupTime,
	ERootMotionFinishVelocityMode VelocityOnFinishMode,
	FVector SetVelocityOnFinish,
	float ClampVelocityOnFinish,
	bool bEnableGravity)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

	US1AbilityTask_DelayedRootMotionForce* MyTask = NewAbilityTask<US1AbilityTask_DelayedRootMotionForce>(OwningAbility, TaskInstanceName);

	MyTask->ForceName			= TaskInstanceName;
	MyTask->WorldDirection		= WorldDirection.GetSafeNormal();
	MyTask->Strength			= Strength;
	MyTask->Duration			= Duration;
	MyTask->WindupTime			= WindupTime;
	MyTask->bIsAdditive			= false;
	MyTask->FinishVelocityMode	= VelocityOnFinishMode;
	MyTask->FinishSetVelocity	= SetVelocityOnFinish;
	MyTask->FinishClampVelocity	= ClampVelocityOnFinish;
	MyTask->bEnableGravity		= bEnableGravity;
	MyTask->SharedInitAndApply();

	return MyTask;
}

void US1AbilityTask_DelayedRootMotionForce::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (nullptr == ASC || false == ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		LOG_WARNING(TEXT("MovementComponent 없음 — Ability %s / Task %s"),
			Ability ? *Ability->GetName() : TEXT("NULL"), *InstanceName.ToString());
		return;
	}

	MovementComponent	= Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
	StartTime			= GetWorld()->GetTimeSeconds();
	EndTime				= StartTime + Duration;

	if (false == MovementComponent.IsValid())
	{
		return;
	}

	TSharedPtr<FS1RootMotionSource_DelayedConstantForce> Force = MakeShared<FS1RootMotionSource_DelayedConstantForce>();
	Force->InstanceName							= ForceName;
	Force->AccumulateMode						= ERootMotionAccumulateMode::Override;
	Force->Priority								= DelayedForcePriority;
	Force->Force								= WorldDirection * Strength;
	Force->Duration								= Duration;
	Force->WindupTime							= WindupTime;
	Force->FinishVelocityParams.Mode			= FinishVelocityMode;
	Force->FinishVelocityParams.SetVelocity		= FinishSetVelocity;
	Force->FinishVelocityParams.ClampVelocity	= FinishClampVelocity;

	if (bEnableGravity)
	{
		// XY만 루트모션, Z는 물리에 맡긴다
		Force->Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
	}

	RootMotionSourceID = MovementComponent->ApplyRootMotionSource(Force);
}

void US1AbilityTask_DelayedRootMotionForce::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(US1AbilityTask_DelayedRootMotionForce, WindupTime);
}
