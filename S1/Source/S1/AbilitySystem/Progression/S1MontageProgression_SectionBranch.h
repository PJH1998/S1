// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "S1MontageProgression_SectionBranch.generated.h"

// BeginSection 재생 → OnBranchRequested(SectionName) 호출 시 해당 섹션으로 이동 → EndAbility
// GA가 Air/Ground 등 분기를 직접 판단해 섹션 이름을 전달
UCLASS()
class S1_API US1MontageProgression_SectionBranch : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnBranchRequested(FName InSectionName) override;
	virtual bool WillHandleAbilityEnd() const override { return true; }

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	// Begin 몽타주에서 시작할 섹션명
	UPROPERTY(EditDefaultsOnly, Category = "SectionBranch")
	FName BeginSection = FName("Begin");

	// 몽타주 에디터에서 Begin에 기본 연결된 섹션 — 같은 섹션 요청 시 점프 생략
	UPROPERTY(EditDefaultsOnly, Category = "SectionBranch")
	FName DefaultEndSection;

	// 섹션 전환(비기본 섹션) 시 Inertialization 블렌드 시간 (0이면 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "SectionBranch")
	float InertializationBlendTime = 0.2f;

	bool bBranched = false;
};
