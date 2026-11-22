// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression.generated.h"

class US1GameplayAbility_Action;
class UAnimMontage;

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
	// 활성화마다 증가하는 세대 번호. 몽타주 콜백은 바인딩 시점의 번호를 payload로 받아
	// 자기 세대가 맞는지 확인한다.
	//
	// 왜 필요한가: GA가 InstancedPerActor라 Progression이 재사용되는데, Montage_Stop의
	// 블렌드아웃이 끝나기 전에 새 활성화가 들어오면 이전 활성화의 콜백이 뒤늦게 발화해
	// 방금 시작한 어빌리티를 끝내버린다. "현재 몽타주인가"로 거르던 기존 방식은 새 활성화가
	// 같은 애셋을 재생하면 통과하고(섹션 모드는 몽타주가 아예 하나다), 그래서 애셋이 아니라
	// 세대로 판별한다.
	int32 ActivationEpoch = 0;

	bool IsCurrentEpoch(int32 InEpoch) const { return InEpoch == ActivationEpoch; }

	TWeakObjectPtr<US1GameplayAbility_Action> GA;
};
