// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "Components/ActorComponent.h"
#include "S1BossLocomotionComponent.generated.h"

class AAIController;
class AS1BossBase;
class UCharacterMovementComponent;
class US1AnimInstance_BossBase;

UENUM(BlueprintType)
enum class EBossApproachState : uint8
{
	Idle,
	Approaching,
	Stopping,
	StopComplete,
};

UENUM(BlueprintType)
enum class EBossTurnState : uint8
{
	Idle,
	Turning,
	TurnComplete,
};

UCLASS(ClassGroup=(Boss), meta=(BlueprintSpawnableComponent))
class S1_API US1BossLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1BossLocomotionComponent();

	bool StartApproach(AActor* Target, EBossLocomotionMode Mode);
	void AbortApproach(bool bInstantReset = true);
	void NotifyLocomotionStopFinished();

	bool StartTurn(AActor* Target);
	void AbortTurn(bool bInstantReset = true);
	void NotifyTurnFinished();

	EBossApproachState GetApproachState() const { return ApproachState; }
	bool IsApproachComplete() const { return ApproachState == EBossApproachState::StopComplete; }

	EBossTurnState GetTurnState() const { return TurnState; }
	bool IsTurnComplete() const { return TurnState == EBossTurnState::TurnComplete; }
	EBossTurnDirection GetTurnDirection() const { return CurrentTurnDirection; }

	bool IsTargetForward(AActor* Target) const;
	EBossTurnDirection ClassifyTurnDirection(AActor* Target) const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void TickApproach(float DeltaTime);
	void BeginStop();
	void RestoreMovementSpeed();
	float GetForwardAngleThresholdCos() const;

	AS1BossBase* GetBossOwner() const;
	AAIController* GetBossAIController() const;
	UCharacterMovementComponent* GetBossMovement() const;
	US1AnimInstance_BossBase* GetBossAnimInstance() const;
	float GetTargetMoveSpeed() const;
	bool IsMoveLocomotionMode(EBossLocomotionMode Mode) const;

private:
	UPROPERTY(EditAnywhere, Category = "Locomotion|Approach", meta = (ClampMin = "0.0"))
	float ChaseStopDistance = { 500.f };

	UPROPERTY(EditAnywhere, Category = "Locomotion|Approach|Run", meta = (ClampMin = "0.0"))
	float RunSpeed = { 600.f };

	UPROPERTY(EditAnywhere, Category = "Locomotion|Approach|Walk", meta = (ClampMin = "0.0"))
	float WalkSpeed = { 300.f };

	UPROPERTY(EditAnywhere, Category = "Locomotion|Approach", meta = (ClampMin = "0.0"))
	float MoveAcceptRadius = { 50.f };

	UPROPERTY(EditAnywhere, Category = "Locomotion|Approach", meta = (ClampMin = "0.0"))
	float AccelerationInterpSpeed = { 3.f };

	UPROPERTY(EditAnywhere, Category = "Locomotion|Turn", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float TurnForwardFov = { 45.f };

	TWeakObjectPtr<AActor> ApproachTarget;
	EBossLocomotionMode LocomotionMode = EBossLocomotionMode::Run;
	EBossApproachState ApproachState = EBossApproachState::Idle;
	EBossTurnState TurnState = EBossTurnState::Idle;
	EBossTurnDirection CurrentTurnDirection = EBossTurnDirection::None;
	float CachedMaxWalkSpeed = 0.f;
};
