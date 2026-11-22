// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "S1GameplayAbility_Hit.generated.h"

class AS1Character;

// 피격 리액션 공통 베이스 (Weak/Strong/ToAir) — GameplayEvent(HitType 태그)로 트리거
// 피격 판정은 서버만 아는 사건이지만 리액션 재생/넉백은 소유 클라에서도 돌아야 한다 → ServerInitiated.
// 넉백 방향/세기는 양쪽이 TriggerEventData->Instigator로 각자 계산하므로 별도 전송 채널이 없다(레이스 없음)
UCLASS(Abstract)
class S1_API US1GameplayAbility_Hit : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Hit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 몽타주 재생 직전 훅 — 여기서 HitLaunch 노티파이를 스캔해 넉백 루트모션 태스크를 만든다
	virtual void OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate, FName SectionName) override;

	// 서브클래스가 ActivateAbility에서 호출 — 넉백 속도를 캐시하고 회전 스냅/Orient 오버라이드를 건다.
	// 몽타주 재생은 Progression이 담당하므로 계산 시점과 사용 시점이 분리되어 멤버로 보관한다
	void CacheHitLaunch(const FVector& InLaunchVelocity, const FVector& InFacingDirection);

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

private:
	// 몽타주의 HitLaunch 노티파이를 데이터 마커로 스캔해 넉백 루트모션 태스크를 만든다
	void ApplyHitLaunchRootMotion(UAnimMontage* Montage, float Rate);

	// Walking 모드는 매 틱 Z를 바닥으로 되돌려 포물선 넉백이 반영되지 않는다 → 힘이 시작되는 순간 Falling으로
	void EnterFallingForHitLaunch();

	FVector CachedLaunchVelocity = FVector::ZeroVector;

	// Progression이 Start/Loop/End를 연달아 재생하면 훅이 여러 번 불리므로 활성화당 1회로 제한
	bool bHitLaunchApplied = false;

	FTimerHandle HitLaunchFallingTimer;
};
