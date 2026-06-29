// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotifyState_MoveEvent.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_MoveEvent : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// 이동 거리 — GA가 활성화 시점에 몽타주를 스캔해 읽는 데이터 (LocalPredicted 예측 루트모션용)
	float GetMoveDistance() const { return MoveDistance; }

private:
	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	FGameplayTag MoveBeginEventTag;

	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	FGameplayTag MoveEndEventTag;

	// 이 구간 동안 이동할 거리 — (거리 / 구간 길이) 속도로 변환해 Payload.EventMagnitude로 전달
	// 짧은 구간이면 빠르게, 긴 구간이면 천천히 같은 거리 이동. 0이면 이동 없음, 음수면 후방 이동
	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	float MoveDistance = 0.f;

	// true면 이동을 GA의 AbilityTask가 처리 — 노티파이는 MoveBegin/End 이벤트만 보내고 RootMotionSource는 추가 안 함
	// (LocalPredicted 전환된 어빌리티용. ServerOnly 어빌리티는 false로 노티파이가 직접 이동)
	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	bool bUseAbilityTaskMovement = false;
};
