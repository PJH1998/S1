// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "S1RootMotionSource_DelayedConstantForce.generated.h"

// 윈드업 구간이 지난 뒤부터 힘을 적용하는 ConstantForce.
//
// 부모의 StrengthOverTime(UCurveFloat*)으로 동일한 게이트를 만들 수 있지만,
// 런타임에 NewObject로 생성한 커브는 네트워크에서 참조 가능한 객체로 등록되지 않으면
// NetGUID를 통한 참조 복제가 불가능하다.
//
// 시뮬레이티드 프록시는 GA를 실행하지 않고, 서버로부터 복제된 Root Motion Source를
// 이용해 PrepareRootMotion을 직접 수행한다.
// 이때 커브 참조가 nullptr로 전달되면 윈드업 게이트가 적용되지 않아
// 준비 구간부터 이동이 시작되고, 서버와의 위치 오차가 누적된다.
//
// 오차가 NetworkNoSmoothUpdateDistance(384)를 초과하면
// SmoothCorrection이 메시 오프셋 보정을 포기하면서 서버 위치 보정이
// 순간이동처럼 노출될 수 있다.
//
// 게이트에 필요한 정보는 이동을 시작할 시점뿐이므로 float으로 충분하다.
// 커브 참조 대신 값을 직접 저장하고 NetSerialize에 포함하면
// 별도의 UObject 참조 해석 없이 프록시에서도 동일한 게이트를 적용할 수 있다.
USTRUCT()
struct FS1RootMotionSource_DelayedConstantForce : public FRootMotionSource_ConstantForce
{
	GENERATED_USTRUCT_BODY()

	virtual ~FS1RootMotionSource_DelayedConstantForce() {}

	// 소스 시작 후 이 시간(초)이 지나기 전에는 힘을 주지 않는다
	UPROPERTY()
	float WindupTime = 0.f;

	virtual FRootMotionSource*	Clone() const override;
	virtual bool				Matches(const FRootMotionSource* Other) const override;
	virtual void				PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent) override;
	virtual bool				NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual UScriptStruct*		GetScriptStruct() const override;
	virtual FString				ToSimpleString() const override;
};

template<>
struct TStructOpsTypeTraits<FS1RootMotionSource_DelayedConstantForce> : public TStructOpsTypeTraitsBase2<FS1RootMotionSource_DelayedConstantForce>
{
	enum
	{
		WithNetSerializer = true,	// FRootMotionSourceGroup::NetSerializeRMSArray가 STRUCT_NetSerializeNative를 요구
		WithCopy = true
	};
};
