// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance.h"
#include "S1AnimInstance_EnemyLocomotion.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EEnemyLocomotionMode : uint8
{
	None,
	Walk,
	Run,
	Turn,
};

UENUM(BlueprintType)
enum class EEnemyLocomotionPhase : uint8
{
	None,
	Start,
	Loop,
	End,
};

UENUM(BlueprintType)
enum class EEnemyTurnDirection : uint8
{
	None,
	Left,
	Right,
	OverLeft,
	OverRight,
};

UCLASS()
class S1_API US1AnimInstance_EnemyLocomotion : public US1AnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void BeginApproach(EEnemyLocomotionMode InMode);
	bool PlayTurnMontage(UAnimMontage* Montage, FName StartSection = NAME_None);
	void StopTurnMontage();
	bool IsPlayingTurnMontage() const;
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

	UFUNCTION()
	void AnimNotify_DeathFinished();

	UFUNCTION()
	void AnimNotify_BeginDeathPresentation();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EEnemyLocomotionMode LocomotionMode = EEnemyLocomotionMode::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	EEnemyLocomotionPhase LocomotionPhase = EEnemyLocomotionPhase::None;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bLocomotionLoop = false;

	UPROPERTY(BlueprintReadOnly, Category = "Death")
	bool bIsDeadAnim = false;

private:
	TWeakObjectPtr<UAnimMontage> ActiveTurnMontage;
};