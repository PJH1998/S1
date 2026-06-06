// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "S1MontageProgression_SectionBranch.generated.h"

// BeginSection 재생 → OnBranchRequested(SectionName) 호출 시 해당 섹션으로 이동 → EndAbility
UCLASS()
class S1_API US1MontageProgression_SectionBranch : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnBranchRequested(FName SectionName) override;
	virtual bool WillHandleAbilityEnd() const override { return true; }

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, Category = "SectionBranch")
	FName BeginSection = FName("Begin");

	// true면 End 섹션 재생 중 → OnMontageEnded에서 EndAbility 처리
	bool bBranched = false;
};
