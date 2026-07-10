// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "S1GameplayAbility_Hit.generated.h"

class AS1Character;

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

	// 공격원 반대 방향 넉백 + 공격원을 향하는 방향(OutHitDirection) 계산 (Strong/ToAir 공용)
	// 회전 스냅 여부(앞/뒤에 따라 정면 유지 vs 반전)는 호출부(서브클래스)가 bOutHitFromBack으로 직접 결정 — ToAir는 항상 OutHitDirection 그대로 사용
	// 공격원을 못 구하면 정면 피격으로 취급. LaunchSpeed는 호출부(서브클래스)가 직접 들고 있는 고정값(cm/s) — 공격 쪽에서 전달받지 않음
	void ComputeKnockback(float LaunchSpeed, bool& bOutHitFromBack, FVector& OutHitDirection, FVector& OutLaunchVelocity) const;

	// AS1Character 캐스트 + MontageProgression 유효성 검사 — 실패 시 EndAbility 후 nullptr 반환 (Strong/ToAir 공용)
	AS1Character* GetValidCharacterOrEnd(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	// MontageProgression->Init(this) + OnActivated() — Weak/Strong/ToAir 공용 마무리 호출
	void StartMontageProgression();

	// 공격원 액터 (TriggerEventData->Instigator) — 서브클래스가 넉백 방향 계산에 사용. Instigator가 const AActor*라 여기도 const로 맞춤
	TWeakObjectPtr<const AActor> AttackerActor;
};
