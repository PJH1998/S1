// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/Repeat/S1MontageProgression_RepeatLoop.h"
#include "S1MontageProgression_RepeatLoopBranch.generated.h"

// 키업 시 SelectEndMontage()로 End 분기 결정
// _RepeatLoop에서 LoopCount / MaxLoopCount / ShouldExitLoop / bImmediateExitOnKeyUp 상속
UCLASS()
class S1_API US1MontageProgression_RepeatLoopBranch : public US1MontageProgression_RepeatLoop
{
	GENERATED_BODY()

public:
	virtual void OnInputReleased() override;

protected:
	// 키업 / MaxLoopCount 자동 탈출 시 재생할 End 몽타주 — 서브클래스에서 override
	virtual UAnimMontage* SelectEndMontage() const;
	virtual UAnimMontage* GetAutoExitMontage() const override { return SelectEndMontage(); }

	// End_B 슬롯 — 서브클래스에서 필요 시 추가
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> EndMontage_B;
};
