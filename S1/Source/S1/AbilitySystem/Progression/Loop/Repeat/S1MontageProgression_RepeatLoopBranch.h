// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "S1MontageProgression_RepeatLoopBranch.generated.h"

// GA 드리븐 End 분기 — 몽타주 기반
// 흐름: Start → Loop 반복 → GA가 조건 판단 → RequestEnd(Key) 직접 호출 → EndMontages[Key] 재생
// 키 업: OnInputReleased → BranchExitEventTag 이벤트 발송 → GA 수신 → RequestEnd(Key) 결정
UCLASS()
class S1_API US1MontageProgression_RepeatLoopBranch : public US1MontageProgression_Loop
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnInputReleased() override;

	// GA가 직접 호출 — InKey에 해당하는 End 섹션으로 전환
	UFUNCTION(BlueprintCallable, Category = "Loop|Branch")
	void RequestEnd(FGameplayTag InKey);

	int32 GetLoopCount() const { return LoopCount; }

protected:
	virtual void OnLoopCycleCompleted() override;

	// Key → End 몽타주 맵 (BP 에디터에서 행 추가로 End 변형 확장)
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> EndMontages;

	// GA가 WaitGameplayEvent로 수신 → RequestEnd 호출 트리거
	UPROPERTY(EditDefaultsOnly, Category = "Loop|Branch")
	FGameplayTag BranchExitEventTag;

private:
	int32 LoopCount = 0;
};
