// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_SequenceBranch.generated.h"

// Begin 몽타주 재생 → GA가 OnBranchRequested(Key) 호출 시 EndMontages[Key] 재생 → EndAbility
// Begin→End 전환: BeginMontage BlendOut=0 설정 필수 (기본 0.25면 idle 비침)
// Air/Ground 등 분기 조건은 GA가 직접 판단해 태그로 전달
UCLASS()
class S1_API US1MontageProgression_SequenceBranch : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnBranchRequested(FGameplayTag InKey) override;
	virtual bool WillHandleAbilityEnd() const override { return true; }

private:
	UFUNCTION()
	void OnEndMontageEnded(UAnimMontage* InMontage, bool bInterrupted);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> BeginMontage;

	// Key → End 몽타주 맵 (BP 에디터에서 행 추가로 분기 확장)
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> EndMontages;

	TObjectPtr<UAnimMontage> CurrentEndMontage;
};
