// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "S1MontageProgression_RepeatLoop.generated.h"

// 사이클 카운트 기반 루프
// 탈출 조건 (2가지):
//   1. 키 업 → OnInputReleased → ExitLoop() 직접 호출
//   2. 사이클 완료 후 LoopCount >= MaxLoopCount → ShouldExitLoop() → ExitLoop()
UCLASS()
class S1_API US1MontageProgression_RepeatLoop : public US1MontageProgression_Loop
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnInputReleased() override;

protected:
	virtual void OnLoopCycleCompleted() override;
	virtual bool ShouldExitLoop() const override;

	// 최대 반복 횟수 — 도달 시 사이클 완료 후 자동 탈출
	UPROPERTY(EditDefaultsOnly, Category = "RepeatLoop")
	int32 MaxLoopCount = 3;

	// true: 키업 시 즉시 End로 점프 / false: 현재 Loop 섹션 끝난 후 End로
	UPROPERTY(EditDefaultsOnly, Category = "RepeatLoop")
	bool bImmediateExitOnKeyUp = false;

	int32 LoopCount = 0;
};
