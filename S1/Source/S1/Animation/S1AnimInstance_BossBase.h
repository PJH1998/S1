// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance.h"
#include "S1AnimInstance_BossBase.generated.h"

UENUM(BlueprintType)
enum class EBossLocomotionMode : uint8
{
	None,
	Walk,
	Run,
	Turn,
};

UENUM(BlueprintType)
enum class EBossLocomotionPhase : uint8
{
	None,
	Start,
	Loop,
	End,
};

UENUM(BlueprintType)
enum class EBossTurnDirection : uint8
{
	None,
	Left,
	Right,
	OverLeft,
	OverRight,
};

UCLASS()
class S1_API US1AnimInstance_BossBase : public US1AnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void BeginApproach(EBossLocomotionMode InMode);
	void BeginTurn(EBossTurnDirection InDirection);
	void RequestStop();
	void ResetLocomotion();
	void SetDeadAnimState(bool bInDead);
	bool IsLocomotionEndPlaying() const;

	UFUNCTION()
	void AnimNotify_LocomotionLoopStart();

	UFUNCTION()
	void AnimNotify_LocomotionEndFinished();

	UFUNCTION()
	void AnimNotify_TurnFinished();

	/** Death 시퀀스 끝: 포즈 고정 + 연출 시작(ABP Notify에서 호출). */
	UFUNCTION()
	void AnimNotify_DeathFinished();

	/** 연출(페이드)만 시작. DeathFinished와 분리 배치 가능. */
	UFUNCTION()
	void AnimNotify_BeginDeathPresentation();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossLocomotionMode LocomotionMode = EBossLocomotionMode::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossLocomotionPhase LocomotionPhase = EBossLocomotionPhase::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossTurnDirection TurnDirection = EBossTurnDirection::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bLocomotionLoop = false;

	/** ABP Dead 분기용. NotifyDeath에서 true. */
	UPROPERTY(BlueprintReadOnly, Category = "Death")
	bool bIsDeadAnim = false;
};
