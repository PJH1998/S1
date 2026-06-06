// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression.generated.h"

class US1GameplayAbility_Action;
struct FS1MontageSet;

UCLASS(Abstract, EditInlineNew)
class S1_API US1MontageProgression : public UObject
{
	GENERATED_BODY()

public:
	void Init(US1GameplayAbility_Action* InGA);

	virtual void				OnActivated() {}
	virtual void				OnDeactivated() {}
	virtual bool				OnInputReactivated() { return false; }
	virtual bool				OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) { return false; }
	virtual FGameplayTag		GetInputFlushTag() const { return FGameplayTag(); }
	virtual const FS1MontageSet* GetCurrentMontageSet() const;

	// true면 Progression이 GA 종료를 직접 처리 (e.g. Landing: 몽타주 끝날 때 EndAbility)
	// false면 GA가 대쉬 Task 종료 시점에 EndAbility
	virtual bool WillHandleAbilityEnd() const { return false; }

	// GA가 외부 조건(이동 종료 등)으로 분기를 요청할 때 호출
	virtual void OnBranchRequested(FName SectionName) {}

protected:
	TWeakObjectPtr<US1GameplayAbility_Action> GA;
};
