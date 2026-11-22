// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "S1AbilityTask_DelayedRootMotionForce.generated.h"

// 엔진 ApplyRootMotionConstantForce와 동일하되, 구간 게이트를 UCurveFloat*가 아니라
// float(초)로 전달한다. 커브 포인터는 시뮬레이티드 프록시까지 복제되지 않아 프록시에서
// 게이트가 사라진다 — 이유는 FS1RootMotionSource_DelayedConstantForce 주석 참조.
//
// Tick/종료/정리는 전부 부모 구현을 쓰고, 소스를 만드는 SharedInitAndApply만 교체한다.
UCLASS()
class S1_API US1AbilityTask_DelayedRootMotionForce : public UAbilityTask_ApplyRootMotionConstantForce
{
	GENERATED_BODY()

public:
	// WindupTime(초)이 지난 뒤부터 Duration 끝까지 WorldDirection * Strength를 적용한다
	static US1AbilityTask_DelayedRootMotionForce* ApplyDelayedRootMotionConstantForce(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FVector WorldDirection,
		float Strength,
		float Duration,
		float WindupTime,
		ERootMotionFinishVelocityMode VelocityOnFinishMode,
		FVector SetVelocityOnFinish,
		float ClampVelocityOnFinish,
		bool bEnableGravity);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void SharedInitAndApply() override;

	UPROPERTY(Replicated)
	float WindupTime = 0.f;
};
