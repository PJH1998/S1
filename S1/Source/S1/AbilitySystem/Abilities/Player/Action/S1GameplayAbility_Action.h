// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Action.generated.h"

class US1MontageProgression;
class US1AnimInstance;
class UAbilityTask_ApplyRootMotionConstantForce;

// 액션 중 상태(State.Action) 관리 + EarlyMove(이동/점프 입력 시 조기 종료) 공통 베이스
// Attack, Dash, Dodge 등 액션 GA의 부모 클래스
UCLASS()
class S1_API US1GameplayAbility_Action : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Action(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// EarlyMove 구간 진입 — 이동/점프 입력 시 Montage_Stop + EndAbility
	virtual void OnEarlyMoveEnabled();

public:
	virtual bool			OnInputReactivated() override;
	virtual void			OnInputReleased() override;
	virtual bool			OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag	GetInputFlushTag() const override;

	// Progression에서 호출하는 GA 공개 API
	void RequestEndAbility(bool bWasCancelled = false);
	void RequestReactivateSelf();
	void RequestActivateAbilityByTag(const FGameplayTagContainer& Tags);

	// 콤보 어드밴스 시 서브클래스 훅 (e.g. GA_Attack: 카메라 회전 갱신)
	virtual void OnProgressionMontageStarted() {}

	// Loop 시작 훅 (e.g. GA_LoopAttack_Dive: 하강 중력 적용)
	virtual void OnProgressionLoopStarted() {}

	// Loop 종료 직전 훅 (e.g. GA_LoopAttack_Dive: 중력 리셋)
	virtual void OnProgressionLoopEnded() {}

	// protected인 GetAnimInstance를 Progression에서 호출 가능하도록 노출
	US1AnimInstance* GetAnimInstanceForProgression() const;

private:
	UFUNCTION()
	void OnEarlyMoveEventReceived(FGameplayEventData Payload);

	void OnEarlyMoveTriggered();

protected:
	// 몽타주 진행 전략 — 에디터에서 인라인 선택/편집
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Action|Montage")
	TObjectPtr<US1MontageProgression> MontageProgression;

	// NotifyState_MoveEvent와 연동 — MoveBegin/End 이벤트 수신 시 OnMoveBeginReceived/OnMoveEndReceived 호출
	// 서브클래스(GA_Evasion, GA_Assault 등)에서 override하여 동작 커스터마이징
	UPROPERTY(EditDefaultsOnly, Category = "Action|Move")
	FGameplayTag MoveBeginEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Action|Move")
	FGameplayTag MoveEndEventTag;

	// MoveBegin → ConstantForce 시작 / MoveEnd → 태스크 종료
	// 서브클래스(GA_Evasion, GA_Assault)에서 override 시 방향/중력 등 커스터마이징
	virtual void OnMoveBeginReceived(const FGameplayEventData* Payload);
	virtual void OnMoveEndReceived(const FGameplayEventData* Payload);

	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MoveTask;

private:
	// virtual dispatch를 위한 non-virtual 래퍼 (GenericGameplayEventCallbacks에 저장됨)
	void InternalMoveBeginCallback(const FGameplayEventData* Payload);
	void InternalMoveEndCallback(const FGameplayEventData* Payload);

	// 액션 중 상태 태그 (ex. State.Action)
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FGameplayTag ActionStateTag;

	// 이 이벤트 수신 시 OnEarlyMoveEnabled 호출 (empty = EarlyMove 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "Action|EarlyMove")
	FGameplayTag EarlyMoveEventTag;

	// 점프 감지용 이벤트 태그 — Character::Jump()에서 SendGameplayEventToActor로 전송
	// (empty = 점프 감지 안 함)
	UPROPERTY(EditDefaultsOnly, Category = "Action|EarlyMove")
	FGameplayTag JumpEventTag;
};
