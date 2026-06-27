// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1ANS_WallKickDash.generated.h"

class AS1Boss_000;

USTRUCT()
struct FS1WallKickDashState
{
	GENERATED_BODY()

	FVector StartLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	float ElapsedTime = 0.f;
	float TotalDuration = 0.f;
};

/**
 * Wall-Kick 이동 노티.
 * 노티 윈도우 동안 보스를 현재 위치에서 Monster->GetWallKickTarget()(기둥 소켓 또는 플레이어)으로 직선 보간 이동한다.
 * 중력 off + 타깃 방향 yaw 정렬. 타깃이 서버에서만 설정되므로 이동은 서버 권한에서만 수행(클라는 CharacterMovement 복제로 동기화).
 */
UCLASS()
class S1_API US1ANS_WallKickDash : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void MoveTo(AS1Boss_000* Monster, const FVector& DesiredLocation) const;
	void Finish(USkeletalMeshComponent* MeshComp, AS1Boss_000* Monster, FS1WallKickDashState& State);

	// 이동 가속 곡선 지수(1=선형, 클수록 초반 가속). 빠른 직선이므로 낮게.
	UPROPERTY(EditAnywhere, Category = "WallKickDash", meta = (ClampMin = "1.0", ClampMax = "6.0"))
	float MoveExponent = 1.5f;

	// 타깃을 바라보게 yaw 정렬.
	UPROPERTY(EditAnywhere, Category = "WallKickDash")
	bool bFaceTarget = true;

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FS1WallKickDashState> ActiveDashes;
};
