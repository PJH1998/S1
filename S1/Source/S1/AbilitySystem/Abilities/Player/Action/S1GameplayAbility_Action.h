// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Action.generated.h"

class US1MontageProgression;
class US1AnimInstance;
class UAbilityTask_ApplyRootMotionConstantForce;
class UAnimMontage;

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

	// 몽타주를 ASC 경유로 재생 — RepAnimMontageInfo로 소유 클라 포함 전체 복제
	// (PlayMontageAndWait 태스크와 동일 메커니즘. 반환: 재생 시간, 실패 시 0)
	float PlayAbilityMontage(UAnimMontage* Montage, FName StartSection = NAME_None, float Rate = 1.f);

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

	// 캡처된 이동 방향 — 기본 수평(CapturedActionMoveDir). pitch 포함 3D 이동이 필요한 서브클래스는 override
	virtual FVector GetCapturedMoveDirection() const;

	// 몽타주의 MoveEvent 노티파이(데이터)를 읽어 활성화 시점(예측 스코프)에 루트모션 task 생성
	// StrengthOverTime 커브로 노티파이 구간에만 force 적용 → 예측키 확보 + 구간 타이밍 정확
	// Rate × Montage RateScale 배속을 반영해 montage-time을 real-time으로 변환
	void ApplyMontageRootMotion(UAnimMontage* Montage, float Rate);

	// 이동 루트모션 중력 적용 — true면 XY만 루트모션/Z는 물리(§36). 자체 중력 제어(GravityScale=0) 서브클래스는 false
	UPROPERTY(EditDefaultsOnly, Category = "Action|Move")
	bool bMoveEnableGravity = true;

	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MoveTask;

	// 활성화 시점에 1회 캡처한 이동 방향 (컨트롤 회전=카메라 기준, 복제되어 클라/서버 일치)
	// 노티파이 시점 실시간 방향은 예측 회전 랙으로 어긋나 서버 보정 되돌림 발생 → 캡처값 사용
	FVector CapturedActionMoveDir = FVector::ForwardVector;

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
