// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_Sequence.generated.h"

class UAbilitySystemComponent;
class UAbilityTask_PlayMontageAndWait;

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
	void	PlayComboAtIndex();
	bool	TryAdvanceCombo();
	bool	CanNextAction(UAbilitySystemComponent* ASC) const;

	int32	GetComboCount() const;

	// 이전 활성화에서 남은 태스크를 바인딩까지 끊고 정리한다.
	// 콤보 1타에서 태스크 1개만 만들고 2타+는 같은 몽타주 안에서 섹션 점프하므로,
	// 콤보 진행 중에는 호출되지 않는다 (활성화/비활성화 시점 전용)
	// 몽타주를 PlayMontageAndWait 태스크로 재생하고 종료 델리게이트를 연결한다
	void	StartMontageTask(UAnimMontage* Montage, FName SectionName = NAME_None);

	void	ReleaseMontageTask();

	// 태스크 델리게이트는 무인자다. 어느 몽타주였는지는 태스크가 자기 것만 보므로 불필요하고,
	// 어빌리티 종료 시 태스크가 델리게이트를 직접 끊으므로(StopPlayingMontage) 지연 콜백 가드도 불필요하다
	UFUNCTION()
	void	OnMontageFinished();

	UFUNCTION()
	void	OnMontageAborted();

private:
	// 콤보 진행에 쓰는 몽타주 — 타수를 섹션으로 나눠 담는다.
	// ⚠ 소유 클라에서 도는 GA 전용 — 섹션 점프가 ASC 경유라 ServerOnly에선 조용히 무시된다
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	TObjectPtr<UAnimMontage> SectionMontage;

	// 콤보 순서대로의 섹션 이름. 비워두면 몽타주 전체를 1타로 재생한다(1타짜리 리액션 등)
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
	TArray<FName> SectionNames;

	// 섹션 점프 시 Inertialization 블렌드 시간 (0이면 비활성)
	// 섹션 점프는 하드컷 — 몽타주 분리와 달리 Blend In/Out이 안 붙는다
	UPROPERTY(EditDefaultsOnly, Category = "Sequence")
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

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;

	int32 CurrentComboIndex = 0;
};
