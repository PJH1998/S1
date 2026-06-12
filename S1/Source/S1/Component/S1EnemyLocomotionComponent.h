// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "Components/ActorComponent.h"
#include "S1EnemyLocomotionComponent.generated.h"

class AAIController;
class AS1Monster;
class UCharacterMovementComponent;
class US1AnimInstance_EnemyLocomotion;

UENUM(BlueprintType)
enum class EEnemyApproachState : uint8
{
	Idle,
	Approaching,
	Stopping,
	StopComplete,
};

UCLASS(ClassGroup=(Enemy), meta=(BlueprintSpawnableComponent))
class S1_API US1EnemyLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1EnemyLocomotionComponent();

	bool StartApproach(AActor* Target, EEnemyLocomotionMode Mode);
	void AbortApproach(bool bInstantReset = true);
	void NotifyLocomotionStopFinished();

	EEnemyApproachState GetApproachState() const { return ApproachState; }
	bool IsApproachComplete() const { return ApproachState == EEnemyApproachState::StopComplete; }

	EEnemyTurnDirection ClassifyTurnDirection(AActor* Target, float ForwardFovDegrees) const;
	bool IsTargetForward(AActor* Target, float ForwardFovDegrees) const;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void TickApproach(float DeltaTime);
	void BeginStop();
	void RestoreMovementSpeed();

	AS1Monster* GetMonsterOwner() const;
	AAIController* GetMonsterAIController() const;
	UCharacterMovementComponent* GetMonsterMovement() const;
	US1AnimInstance_EnemyLocomotion* GetEnemyAnimInstance() const;
	float GetTargetMoveSpeed() const;
	bool IsMoveLocomotionMode(EEnemyLocomotionMode Mode) const;

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

	TWeakObjectPtr<AActor> ApproachTarget;
	EEnemyLocomotionMode LocomotionMode = EEnemyLocomotionMode::Run;
	EEnemyApproachState ApproachState = EEnemyApproachState::Idle;
	float CachedMaxWalkSpeed = 0.f;
};