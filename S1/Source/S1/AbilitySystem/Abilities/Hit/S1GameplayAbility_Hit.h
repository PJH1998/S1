// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "S1GameplayAbility_Hit.generated.h"

// 피격 리액션 공통 베이스 (Weak/Strong/ToAir) — GameplayEvent(HitType 태그)로 트리거
// 피격은 서버만 아는 사건이라 예측할 클라가 없음 → 항상 ServerOnly (LocalPredicted 전환 대상 아님)
UCLASS(Abstract)
class S1_API US1GameplayAbility_Hit : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Hit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 공격원 반대 방향 넉백 + 앞/뒤 회전 분기 계산 (Strong/ToAir 공용)
	// 앞 피격: 공격원을 정면으로 바라봄 / 뒤 피격: 공격원을 등지고(뒤에 두고) 바라봄 — 넉백은 두 경우 다 공격원 반대 방향(월드 기준)으로 동일
	// 공격원을 못 구하면 정면 피격으로 취급. LaunchSpeed는 호출부(서브클래스)가 직접 들고 있는 고정값(cm/s) — 공격 쪽에서 전달받지 않음
	void ComputeKnockback(float LaunchSpeed, bool& bOutHitFromBack, FVector& OutFacingDirection, FVector& OutLaunchVelocity) const;

	// 공격원 액터 (TriggerEventData->Instigator) — 서브클래스가 넉백 방향 계산에 사용. Instigator가 const AActor*라 여기도 const로 맞춤
	TWeakObjectPtr<const AActor> AttackerActor;
};
