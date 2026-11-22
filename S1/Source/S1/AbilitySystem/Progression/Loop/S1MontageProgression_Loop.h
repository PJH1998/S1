// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_Loop.generated.h"

class UAbilityTask_PlayMontageAndWait;

// 한 몽타주의 섹션으로 Start → Loop(반복) → End 를 진행한다.
//
// 반복과 전환은 섹션 Next 링크를 런타임에 바꿔서 처리한다 — 몽타주를 갈아끼우지 않으므로
// PlayMontageAndWait 태스크 하나가 어빌리티 끝까지 살아 있고, 지연 콜백(epoch) 문제가 없다.
//     활성화     SetMontageNextSection(Start, Loop) / (Loop, Loop) / (End, None)
//     사이클     LoopCycleEventTag 노티파이 → count/이펙트 → 탈출 조건이면 (Loop, End)로 변경
//     탈출       ExitLoop() → 즉시면 JumpToMontageSection(End), 아니면 Next를 End로
//
// ⚠ LoopCycleEventTag 노티파이가 필수다. 섹션 전환에는 몽타주 BlendingOut이 없어서
//   예전의 BlendingOut 폴백 경로가 성립하지 않는다. Loop 섹션 끝 직전에 배치할 것.
UCLASS()
class S1_API US1MontageProgression_Loop : public US1MontageProgression
{
	GENERATED_BODY()

public:
	US1MontageProgression_Loop();

	virtual void		 OnActivated() override;
	virtual void		 OnDeactivated() override;
	virtual bool		 OnInputReactivated() override;
	virtual bool		 OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag GetInputFlushTag() const override { return CanNextAttackTag; }
	virtual bool		 WillHandleAbilityEnd() const override { return true; }

protected:
	// EffectTag: OnLoopCycleEventReceived의 Payload.InstigatorTags에서 추출(없으면 Invalid) — Charge류 서브클래스가 사용
	virtual void  OnLoopCycleCompleted(FGameplayTag EffectTag) {}
	virtual bool  ShouldExitLoop() const { return false; }

	// ShouldExitLoop() true 시 흐를 End 섹션 — None이면 EndSectionName 사용
	virtual FName GetAutoExitSectionName() const { return NAME_None; }

	// TargetEndSection이 None이면 EndSectionName 사용
	// bImmediate=true면 Loop 섹션을 즉시 끊고 End로 점프, false면 현재 섹션이 끝난 뒤 End로 흐름
	void ExitLoop(FName TargetEndSection = NAME_None, bool bImmediate = false);

	// 섹션 점프는 하드컷이라 포즈를 직접 이어줘야 한다 (몽타주 분리와 달리 Blend In/Out이 없다)
	void RequestSectionBlend();

	// 섹션 구조를 담은 단일 몽타주 — 서브클래스가 방향 등으로 교체할 수 있어 protected
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> SectionMontage;

	// 비워두면 Start 없이 Loop 섹션부터 시작한다
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName StartSectionName = TEXT("Start");

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName LoopSectionName = TEXT("Loop");

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName EndSectionName = TEXT("End");

	// Start 섹션 끝에 배치한 AnimNotify → Loop 진입 통지 (전환 자체는 섹션 Next가 처리).
	// 비워두면 활성화 즉시 Loop 단계로 간주한다
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopStartEventTag;

	// Loop 섹션 끝 직전에 배치한 AnimNotify → 사이클 완료. ⚠ 필수 (위 클래스 주석 참조)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopCycleEventTag;

	// 외부 이벤트로 탈출 요청 (선택 — 비워두면 키업/MaxCount로만 탈출)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopEndEventTag;

	// true: LoopEndEventTag 수신 즉시 End로 점프 / false: 현재 사이클이 끝난 후 End로
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	bool bImmediateExitOnLoopEnd = false;

	// 섹션 점프/반복 시 Inertialization 블렌드 시간 (0이면 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	float LoopInertializationBlendTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTag CanNextAttackTag;

	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTagContainer CrossComboGroupTags;

	bool bEndRequested = false;
	bool bInLoopPhase  = false;

private:
	void StartMontageTask(FName StartSection);
	void ReleaseMontageTask();
	void EnterLoopPhase();
	void SubscribeLoopCycle();

	void OnLoopStartEventReceived(const FGameplayEventData* Payload);
	void OnLoopCycleEventReceived(const FGameplayEventData* Payload);
	UFUNCTION() void OnLoopEndEventReceived(FGameplayEventData Payload);

	// 태스크 델리게이트는 무인자 — 어빌리티 종료 시 태스크가 델리게이트를 직접 끊으므로 지연 콜백 가드가 불필요하다
	UFUNCTION() void OnMontageFinished();
	UFUNCTION() void OnMontageAborted();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;

	FName CurrentEndSectionName;
};
