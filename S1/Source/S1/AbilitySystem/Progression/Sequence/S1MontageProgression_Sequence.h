// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_Sequence.generated.h"

class UAbilitySystemComponent;

UCLASS()
class S1_API US1MontageProgression_Sequence : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void			OnActivated() override;
	virtual void			OnDeactivated() override;
	virtual bool			OnInputReactivated() override;
	virtual bool			OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag	GetInputFlushTag() const override { return CanNextAttackTag; }

private:
	void	PlayMontageAtIndex();
	bool	TryAdvanceCombo();
	bool	CanNextAction(UAbilitySystemComponent* ASC) const;

	UFUNCTION()
	void	OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	// 콤보 순서대로 — 각 타수가 별도 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	TArray<TObjectPtr<UAnimMontage>> ComboMontages;

	// 콤보 전환 가능 여부 Tag
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTag CanNextAttackTag;

	// 마지막 콤보 이후 전환할 AbilityTag (비어있으면 자기 재발동)
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTag NextAttackAbilityTag;

	// Cross-combo 허용 그룹
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTagContainer CrossComboGroupTags;

	int32 CurrentComboIndex = 0;
};
