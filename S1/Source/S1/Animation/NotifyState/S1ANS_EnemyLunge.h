// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1ANS_EnemyLunge.generated.h"

class ACharacter;
class AS1Monster;
class USkeletalMeshComponent;

USTRUCT()
struct FS1EnemyLungeState
{
	GENERATED_BODY()

	float ElapsedTime = 0.f;
	float TotalDuration = 0.f;
	FVector MoveDirection = FVector::ZeroVector;
	float CachedGravityScale = 1.f;
	bool bHasJumpedToEnd = false;
};

UCLASS()
class S1_API US1ANS_EnemyLunge : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	US1ANS_EnemyLunge();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	static float LungeProgress(float NormalizedTime, float RushExponent, float TailLinearBlend);
	void ApplyMoveDelta(ACharacter* Character, const FVector& MoveDirection, float DeltaDist) const;
	void ApplyLungeDelta(ACharacter* Character, const FS1EnemyLungeState& State, float PrevNormalizedTime, float CurrNormalizedTime) const;
	void FinishLunge(USkeletalMeshComponent* MeshComp, ACharacter* Character, FS1EnemyLungeState& State);
	bool TryJumpToEndOnApproach(USkeletalMeshComponent* MeshComp, AS1Monster* Monster, FS1EnemyLungeState& State) const;

	UPROPERTY(EditAnywhere, Category = "EnemyLunge")
	float MoveDistance = 150.f;

	// 초반 돌진 강도 — 클수록 앞쪽에 거리 집중 (2~3 권장, 가속 구간 없이 바로 빠름)
	UPROPERTY(EditAnywhere, Category = "EnemyLunge", meta = (ClampMin = "1.0", ClampMax = "6.0"))
	float RushExponent = 2.5f;

	// 후반 감속 완만함 — 클수록 끝까지 서서히 줄어듦 (0.35~0.5 권장)
	UPROPERTY(EditAnywhere, Category = "EnemyLunge", meta = (ClampMin = "0.0", ClampMax = "0.8"))
	float TailLinearBlend = 0.45f;

	UPROPERTY(EditAnywhere, Category = "EnemyLunge")
	bool bEnableGravity = true;

	// 캡슐 가장자리 기준 이 거리 이하 접근 시 End 섹션으로 즉시 점프 (0=비활성)
	UPROPERTY(EditAnywhere, Category = "EnemyLunge|ApproachEnd", meta = (ClampMin = "0.0"))
	float ApproachEndDistance = 250.f;

	UPROPERTY(EditAnywhere, Category = "EnemyLunge|ApproachEnd")
	FName EndSectionName = FName("End");

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FS1EnemyLungeState> ActiveLunges;
};
