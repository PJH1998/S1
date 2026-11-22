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
	void	PlaySectionAtIndex();
	bool	TryAdvanceCombo();
	bool	CanNextAction(UAbilitySystemComponent* ASC) const;

	// SectionMontage가 채워져 있으면 섹션 모드 (별도 bool 없이 "뭘 채웠나"로 판별)
	bool	IsSectionMode() const;
	int32	GetComboCount() const;

	void	OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 Epoch);

private:
	// [배열 모드] 콤보 순서대로 — 각 타수가 별도 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	TArray<TObjectPtr<UAnimMontage>> ComboMontages;

	// [섹션 모드] 한 몽타주 안의 섹션으로 콤보 진행. 채우면 ComboMontages 대신 이쪽을 쓴다
	// ⚠ LocalPredicted GA 전용 — 섹션 점프가 ASC 경유라 ServerOnly에선 동작하지 않는다
	UPROPERTY(EditDefaultsOnly, Category = "Sequence|Section")
	TObjectPtr<UAnimMontage> SectionMontage;

	// 콤보 순서대로의 섹션 이름
	UPROPERTY(EditDefaultsOnly, Category = "Sequence|Section")
	TArray<FName> SectionNames;

	// 섹션 점프 시 Inertialization 블렌드 시간 (0이면 비활성)
	// 섹션 점프는 하드컷 — 몽타주 분리와 달리 Blend In/Out이 안 붙는다
	UPROPERTY(EditDefaultsOnly, Category = "Sequence|Section")
	float SectionInertializationBlendTime = 0.f;

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
