// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1AnimNotifyState_HitLaunch.generated.h"

// 피격 넉백(Strong/ToAir) 전용 — S1AnimNotifyState_MoveEvent와 동일한 양쪽(소유 클라+서버) 직접 루트모션 주입 구조
// GA_Hit는 ServerOnly라 ActivateAbility가 서버에서만 실행되므로, 이동은 이 Notify가 각 머신에서 로컬로 담당
// 방향/세기는 AS1Character::PendingHitLaunchVelocity(서버가 세팅, Replicated)를 읽어 결정 — 몽타주마다 고정값이 아니라 공격별로 달라지기 때문
UCLASS()
class S1_API US1AnimNotifyState_HitLaunch : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// true면 구간이 끝나는 순간의 속도를 그대로 유지(ToAir Start 구간 — 이후 중력받아 포물선 낙하)
	// false면 정지(Strong 등 제자리로 끝나는 지상 넉백)
	UPROPERTY(EditAnywhere, Category = "HitLaunch")
	bool bMaintainVelocityOnFinish = false;
};
