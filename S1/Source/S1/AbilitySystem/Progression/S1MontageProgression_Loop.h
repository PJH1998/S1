// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_Loop.generated.h"

UCLASS()
class S1_API US1MontageProgression_Loop : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void		OnActivated() override;
	virtual bool		OnInputReactivated() override;
	virtual bool		OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag GetInputFlushTag() const override { return CanNextAttackTag; }

private:
	UFUNCTION()
	void OnLoopEndEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	// Loop 종료 트리거 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopEndEventTag;

	// 다음 액션 전환 가능 윈도우 태그
	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTag CanNextAttackTag;

	// 전환 허용 Ability 그룹 태그
	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTagContainer CrossComboGroupTags;

	UPROPERTY(EditDefaultsOnly, Category = "Loop|Section")
	FName StartSection = FName("Start");

	UPROPERTY(EditDefaultsOnly, Category = "Loop|Section")
	FName LoopSection = FName("Loop");

	UPROPERTY(EditDefaultsOnly, Category = "Loop|Section")
	FName EndSection = FName("End");
};
