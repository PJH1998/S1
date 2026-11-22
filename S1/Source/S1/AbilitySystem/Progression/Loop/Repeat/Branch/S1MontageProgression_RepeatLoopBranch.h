// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoop.h"
#include "S1MontageProgression_RepeatLoopBranch.generated.h"

// 키업 시 SelectEndSectionName()으로 End 섹션 분기 결정
// _RepeatLoop에서 LoopCount / MaxLoopCount / ShouldExitLoop / bImmediateExitOnKeyUp 상속
UCLASS()
class S1_API US1MontageProgression_RepeatLoopBranch : public US1MontageProgression_RepeatLoop
{
	GENERATED_BODY()

public:
	virtual void OnInputReleased() override;

protected:
	// 키업 / MaxLoopCount 자동 탈출 시 흐를 End 섹션 이름 — 서브클래스에서 override
	virtual FName SelectEndSectionName() const;
	virtual FName GetAutoExitSectionName() const override { return SelectEndSectionName(); }

	// End_B 슬롯 — 서브클래스에서 필요 시 사용 (같은 몽타주 안의 다른 End 섹션)
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName EndSectionName_B;
};