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
	virtual void				OnActivated() override;
	virtual void				OnDeactivated() override;
	virtual bool				OnInputReactivated() override;
	virtual bool				OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag		GetInputFlushTag() const override { return CanNextAttackTag; }
	virtual const FS1MontageSet* GetCurrentMontageSet() const override;

	// OnActivated 전에 호출 시 해당 섹션부터 재생 (1회성 — 재생 후 자동 리셋)
	void SetStartSection(FName Section) { OverrideStartSection = Section; }

private:
	void	PlayMontageAtIndex();
	bool	TryAdvanceCombo();
	bool	CanNextAction(UAbilitySystemComponent* ASC) const;

	UFUNCTION()
	void	OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	// 콤보 전환 가능 여부 Tag
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTag CanNextAttackTag;

	// 마지막 콤보 이후 전환할 AbilityTag
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTag NextAttackAbilityTag;

	// Cross-combo 허용 그룹
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	FGameplayTagContainer CrossComboGroupTags;

	// 전환 조건 태그 — 유효하면 Transition 동작 활성 (비어있으면 일반 콤보)
	UPROPERTY(EditDefaultsOnly, Category = "Sequence|Transition")
	FGameplayTag TransitionStateTag;

	int32 CurrentSectionIndex = 0;
	FName OverrideStartSection = NAME_None;
};
