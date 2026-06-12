// Fill out your copyright notice in the Description page of Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression_Loop.h"
#include "S1MontageProgression_HoldLoop.generated.h"

// Start → Loop → End 단일 몽타주에서 키 홀드 동안 Loop 반복
// 키 업 or MaxLoopCount 도달 시 현재 사이클 완료 후 End 섹션으로 전환 (Loop 최소 1회 보장)
// 몽타주 에셋의 Loop 섹션 시작 지점에 S1AnimNotify_SendGameplayEvent(LoopCycleEventTag) 배치 필요
UCLASS()
class S1_API US1MontageProgression_HoldLoop : public US1MontageProgression_Loop
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnInputReleased() override;

private:
	UFUNCTION()
	void OnLoopCycleEventReceived(FGameplayEventData Payload);

	// Loop 섹션의 다음 연결을 End로 변경 — 현재 사이클은 끝까지 재생
	// Loop 재생 중이 아니면 무시 (Start 중 키 업은 첫 사이클 이벤트에서 처리 — 최소 1회 보장)
	void BranchToEnd();

private:
	// Loop 섹션 시작 노티파이의 이벤트 태그 — 사이클 카운트용
	UPROPERTY(EditDefaultsOnly, Category = "HoldLoop")
	FGameplayTag LoopCycleEventTag;

	// 최대 루프 반복 횟수 — 도달 시 키 홀드 중이어도 End로 전환
	UPROPERTY(EditDefaultsOnly, Category = "HoldLoop")
	int32 MaxLoopCount = 3;

	int32 LoopCount = 0;
	bool  bReleased = false;
	bool  bBranchRequested = false;
};
