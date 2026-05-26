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
	bool IsLocomotionEndPlaying() const;

	UFUNCTION()
	void AnimNotify_LocomotionLoopStart();

	UFUNCTION()
	void AnimNotify_LocomotionEndFinished();

	UFUNCTION()
	void AnimNotify_TurnFinished();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossLocomotionMode LocomotionMode = EBossLocomotionMode::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossLocomotionPhase LocomotionPhase = EBossLocomotionPhase::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EBossTurnDirection TurnDirection = EBossTurnDirection::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bLocomotionLoop = false;
};
