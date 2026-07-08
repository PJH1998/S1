// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_Loop.generated.h"

// 몽타주 기반: StartMontage → LoopMontage (반복) → EndMontage
// 사이클 감지: OnLoopMontageBlendingOut/OnLoopCycleEventReceived → OnLoopCycleCompleted(EffectTag) → ShouldExitLoop()
// 탈출: LoopEndEventTag 이벤트 수신 or 키업(OnInputReleased) → ExitLoop()
// BlendOut=0 필수 (Start/Loop 몽타주) — 기본값 0.25면 idle 비침
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
	virtual void OnLoopCycleCompleted(FGameplayTag EffectTag) {}
	virtual bool ShouldExitLoop() const { return false; }

	// ShouldExitLoop() true 시 재생할 End 몽타주 — nullptr이면 EndMontage 사용
	virtual UAnimMontage* GetAutoExitMontage() const { return nullptr; }

	// TargetEnd가 nullptr이면 EndMontage 사용
	// bImmediate=true면 LoopMontage 즉시 중단 후 End 재생, false면 현재 Loop 끝난 후 End 재생
	void ExitLoop(UAnimMontage* TargetEnd = nullptr, bool bImmediate = false);

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> StartMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> LoopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> EndMontage;

	// Start → Loop 전환 이벤트 (StartMontage 끝 프레임에 AnimNotify로 전달)
	// notify 발화 시점에 LoopMontage 시작 → 팝 없음 + 이전 notify(StartDive 등) 보장
	// 비워두면: BlendingOut 폴백 (StartMontage BlendOut > 0 필요)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopStartEventTag;

	// Loop → Loop/End 전환 이벤트 (LoopMontage 끝 직전 AnimNotify로 전달)
	// 발화 시점에 count 증가 + 키업/MaxCount 체크 → Loop or End 즉시 시작 → 팝 없음
	// 비워두면: BlendingOut 폴백 (LoopMontage BlendOut > 0 필요)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopCycleEventTag;

	// 외부 이벤트로 즉시 탈출 (선택 — 비워두면 키업/MaxCount로만 탈출)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	FGameplayTag LoopEndEventTag;

	// Loop 재시작 시 Inertialization 블렌드 시간 (0이면 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "Loop")
	float LoopInertializationBlendTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTag CanNextAttackTag;

	UPROPERTY(EditDefaultsOnly, Category = "Loop|CrossCombo")
	FGameplayTagContainer CrossComboGroupTags;

	bool bEndRequested  = false;
	bool bInLoopPhase   = false;

private:
	void PlayLoopMontage();
	void PlayEndMontage();

	UFUNCTION() void OnStartMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION() void OnLoopMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION() void OnEndMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnLoopStartEventReceived(const FGameplayEventData* Payload);
	void OnLoopCycleEventReceived(const FGameplayEventData* Payload);
	UFUNCTION() void OnLoopEndEventReceived(FGameplayEventData Payload);

	TObjectPtr<UAnimMontage> CurrentEndMontage;
};
