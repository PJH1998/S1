// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1GameplayAbility_Action.generated.h"

class US1MontageProgression;
class US1AnimInstance;
class UAbilityTask_ApplyRootMotionConstantForce;
struct FS1MontageData;
struct FS1MontageSet;

// 액션 중 상태(State.Action) 관리 + EarlyMove(이동/점프 입력 시 조기 종료) 공통 베이스
// Attack, Dash, Dodge 등 액션 GA의 부모 클래스
UCLASS()
class S1_API US1GameplayAbility_Action : public US1GameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// EarlyMove 구간 진입 — 이동/점프 입력 시 Montage_Stop + EndAbility
	virtual void OnEarlyMoveEnabled();

public:
	virtual bool			OnInputReactivated() override;
	virtual bool			OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag	GetInputFlushTag() const override;

	// Progression에서 호출하는 GA 공개 API
	void RequestEndAbility(bool bWasCancelled = false);
	void RequestReactivateSelf();
	void RequestActivateAbilityByTag(const FGameplayTagContainer& Tags);

	// 콤보 어드밴스 시 서브클래스 훅 (e.g. GA_Attack: 카메라 회전 갱신)
	virtual void OnProgressionMontageStarted() {}

	// Loop 종료 직전 훅 (e.g. GA_LoopAttack_Dive: 중력 리셋)
	virtual void OnProgressionLoopEnded() {}

	const FS1MontageData*	GetMontageData() const;
	// 같은 AnimData 에셋에서 다른 MontageTag로 조회 (End 몽타주 등 분기 재생용)
	const FS1MontageData*	GetMontageDataByTag(FGameplayTag InMontageTag) const;
	virtual const FS1MontageSet* GetCurrentMontageSet() const;

	// protected인 GetAnimInstance를 Progression에서 호출 가능하도록 노출
	US1AnimInstance* GetAnimInstanceForProgression() const;

private:
	UFUNCTION()
	void OnEarlyMoveEventReceived(FGameplayEventData Payload);

	void OnEarlyMoveTriggered();

protected:
	// 실행할 Montage를 보관 중인 AnimData Asset Tag
	UPROPERTY(EditDefaultsOnly, Category = "Action|Montage")
	FGameplayTag AnimDataTag;

	// 무기 비의존 기본 Montage Tag
	// bUseWeaponMontage = true 일 때 MontageTagByWeapon에 현재 무기 타입이 없으면 폴백으로 사용
	UPROPERTY(EditDefaultsOnly, Category = "Action|Montage")
	FGameplayTag MontageTag;

	// true: MontageTagByWeapon에서 현재 장착 무기 WeaponType으로 Montage Tag 결정 (플레이어 무기 GA 전용)
	// false: MontageTag 고정 사용 (Common GA, 몬스터 GA)
	UPROPERTY(EditDefaultsOnly, Category = "Action|Montage")
	bool bUseWeaponMontage = false;

	// 무기 종류별 Montage Tag (bUseWeaponMontage = true 일 때만 참조)
	UPROPERTY(EditDefaultsOnly, Category = "Action|Montage", meta = (EditCondition = "bUseWeaponMontage"))
	TMap<ES1WeaponType, FGameplayTag> MontageTagByWeapon;

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
