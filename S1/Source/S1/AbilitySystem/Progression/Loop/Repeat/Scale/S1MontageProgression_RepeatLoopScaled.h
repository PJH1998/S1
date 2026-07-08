// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoop.h"
#include "S1MontageProgression_RepeatLoopScaled.generated.h"

// 사이클마다 데미지 배율 단계 상승 (DamageScaleStages 배열 순서)
// 즉시 탈출(이벤트) 시 현재 사이클 기준 배율 적용
UCLASS()
class S1_API US1MontageProgression_RepeatLoopScaled : public US1MontageProgression_RepeatLoop
{
	GENERATED_BODY()

public:
	virtual void  OnActivated() override;
	virtual void  OnDeactivated() override;
	virtual float GetDamageMultiplier() const override;

protected:
	virtual void OnLoopCycleCompleted(FGameplayTag EffectTag) override;

private:
	// 사이클 0, 1, 2 ... 순서의 데미지 배율
	UPROPERTY(EditDefaultsOnly, Category = "RepeatLoopScaled")
	TArray<float> DamageScaleStages = { 1.0f, 1.5f, 2.0f, 3.0f };

	int32 CurrentStage = 0;
};
