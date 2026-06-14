// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/Repeat/Branch/S1MontageProgression_RepeatLoopBranch.h"
#include "S1MontageProgression_RepeatLoopBranch_Count.generated.h"

// LoopCount 기반 분기 — Threshold 미만이면 EndMontage, 이상이면 EndMontage_B
// LoopCount / MaxLoopCount / ShouldExitLoop 는 _RepeatLoop에서 상속
UCLASS()
class S1_API US1MontageProgression_RepeatLoopBranch_Count : public US1MontageProgression_RepeatLoopBranch
{
	GENERATED_BODY()

protected:
	virtual UAnimMontage* SelectEndMontage() const override;

	// 키업 시 분기 기준 — 미만이면 EndMontage, 이상이면 EndMontage_B
	UPROPERTY(EditDefaultsOnly, Category = "Loop|Branch")
	int32 Threshold = 1;
};
