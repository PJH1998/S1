// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression.generated.h"

class US1GameplayAbility_Action;

UCLASS(Abstract, EditInlineNew)
class S1_API US1MontageProgression : public UObject
{
	GENERATED_BODY()

public:
	void Init(US1GameplayAbility_Action* InGA);

	virtual void			OnActivated() {}
	virtual void			OnDeactivated() {}
	virtual bool			OnInputReactivated() { return false; }
	virtual void			OnInputReleased() {}
	virtual bool			OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) { return false; }
	virtual FGameplayTag	GetInputFlushTag() const { return FGameplayTag(); }

	// AtkCollision 배율 × Hold 배율 합산 — 기본 1.0, 서브클래스에서 override
	virtual float GetDamageMultiplier() const { return 1.0f; }

	virtual bool WillHandleAbilityEnd() const { return false; }

	// GA가 외부 조건(이동 종료 등)으로 분기를 요청할 때 호출
	// InKey: EndMontages 맵의 키 (GA가 직접 결정해서 전달)
	virtual void OnBranchRequested(FGameplayTag InKey) {}

protected:
	TWeakObjectPtr<US1GameplayAbility_Action> GA;
};
