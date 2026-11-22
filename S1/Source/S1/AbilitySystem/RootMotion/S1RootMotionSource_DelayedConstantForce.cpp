// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/RootMotion/S1RootMotionSource_DelayedConstantForce.h"

FRootMotionSource* FS1RootMotionSource_DelayedConstantForce::Clone() const
{
	return new FS1RootMotionSource_DelayedConstantForce(*this);
}

bool FS1RootMotionSource_DelayedConstantForce::Matches(const FRootMotionSource* Other) const
{
	if (false == Super::Matches(Other))
	{
		return false;
	}

	// Super 체인의 FRootMotionSource::Matches가 ScriptStruct 동일성을 보장하므로 캐스트가 안전하다
	const FS1RootMotionSource_DelayedConstantForce* OtherCast = static_cast<const FS1RootMotionSource_DelayedConstantForce*>(Other);

	return FMath::IsNearlyEqual(WindupTime, OtherCast->WindupTime);
}

void FS1RootMotionSource_DelayedConstantForce::PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
	if (GetTime() < WindupTime)
	{
		// 이동 없음. Clear()는 bHasRootMotion만 내리고 트랜스폼을 남겨두는데
		// AccumulateRootMotionVelocityFromSource는 bHasRootMotion을 보지 않고 트랜스폼을 읽으므로
		// 0 트랜슬레이션을 명시적으로 넣는다 (Override 모드 → 제자리 고정)
		RootMotionParams.Set(FTransform::Identity);
		SetTime(GetTime() + SimulationTime);
		return;
	}

	// StrengthOverTime은 쓰지 않으므로(nullptr) 부모는 등속으로만 동작한다
	Super::PrepareRootMotion(SimulationTime, MovementTickTime, Character, MoveComponent);
}

bool FS1RootMotionSource_DelayedConstantForce::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (false == Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << WindupTime;

	bOutSuccess = true;
	return true;
}

UScriptStruct* FS1RootMotionSource_DelayedConstantForce::GetScriptStruct() const
{
	return FS1RootMotionSource_DelayedConstantForce::StaticStruct();
}

FString FS1RootMotionSource_DelayedConstantForce::ToSimpleString() const
{
	return FString::Printf(TEXT("[ID:%u]FS1RootMotionSource_DelayedConstantForce %s Windup(%.3f)"), LocalID, *InstanceName.GetPlainNameString(), WindupTime);
}
