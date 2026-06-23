// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1EnemyLocomotionComponent.h"
#include "AIController.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "Character/S1Monster.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

US1EnemyLocomotionComponent::US1EnemyLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void US1EnemyLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (AS1Monster* Monster = GetMonsterOwner())
	{
		if (false == Monster->HasAuthority())
		{
			SetComponentTickEnabled(false);
			return;
		}
	}

	if (ApproachState == EEnemyApproachState::Approaching)
	{
		TickApproach(DeltaTime);
	}
}

bool US1EnemyLocomotionComponent::StartApproach(AActor* Target, EEnemyLocomotionMode Mode)
{
	if (AS1Monster* Monster = GetMonsterOwner())
	{
		if (false == Monster->HasAuthority())
		{
			return false;
		}
	}

	if (Target == nullptr || IsMoveLocomotionMode(Mode) == false)
	{
		return false;
	}

	if (US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance())
	{
		AnimInstance->StopTurnMontage();
	}

	if (ApproachState == EEnemyApproachState::Approaching || ApproachState == EEnemyApproachState::Stopping)
	{
		AbortApproach(true);
	}

	AS1Monster* Monster = GetMonsterOwner();
	AAIController* AIController = GetMonsterAIController();
	US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance();
	UCharacterMovementComponent* Movement = GetMonsterMovement();
	if (Monster == nullptr || AIController == nullptr || AnimInstance == nullptr || Movement == nullptr)
	{
		return false;
	}

	ApproachTarget = Target;
	LocomotionMode = Mode;
	ApproachState = EEnemyApproachState::Approaching;
	CachedMaxWalkSpeed = Movement->MaxWalkSpeed;
	Movement->MaxWalkSpeed = FMath::Max(50.f, GetTargetMoveSpeed() * 0.15f);
	AnimInstance->BeginApproach(Mode);
	SyncReplicatedLocomotionState();

	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToActor(Target, MoveAcceptRadius);
	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		AbortApproach(true);
		return false;
	}

	SetComponentTickEnabled(true);

	return true;
}

void US1EnemyLocomotionComponent::AbortApproach(bool bInstantReset)
{
	AAIController* AIController = GetMonsterAIController();
	if (AIController)
	{
		AIController->StopMovement();
	}

	RestoreMovementSpeed();

	if (US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance())
	{
		if (bInstantReset)
		{
			AnimInstance->ResetLocomotion();
		}
		else if (AnimInstance->LocomotionMode != EEnemyLocomotionMode::None)
		{
			AnimInstance->RequestStop();
		}
	}

	SyncReplicatedLocomotionState();

	ApproachTarget = nullptr;
	ApproachState = EEnemyApproachState::Idle;
	SetComponentTickEnabled(false);
}

void US1EnemyLocomotionComponent::NotifyLocomotionStopFinished()
{
	if (ApproachState != EEnemyApproachState::Stopping)
	{
		return;
	}

	ApproachState = EEnemyApproachState::StopComplete;
	ApproachTarget = nullptr;
	RestoreMovementSpeed();
	SetComponentTickEnabled(false);
}

EEnemyTurnDirection US1EnemyLocomotionComponent::ClassifyTurnDirection(AActor* Target, float ForwardFovDegrees) const
{
	AS1Monster* Monster = GetMonsterOwner();
	if (Monster == nullptr || Target == nullptr)
	{
		return EEnemyTurnDirection::None;
	}

	FVector ToTarget = Target->GetActorLocation() - Monster->GetActorLocation();
	ToTarget.Z = 0.f;

	if (ToTarget.Normalize() == false)
	{
		return EEnemyTurnDirection::None;
	}

	FVector Forward = Monster->GetActorForwardVector();
	Forward.Z = 0.f;

	if (Forward.Normalize() == false)
	{
		return EEnemyTurnDirection::None;
	}

	const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);
	const float ForwardDot = FVector::DotProduct(Forward, ToTarget);
	const float RightDot = FVector::DotProduct(Right, ToTarget);
	const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(ForwardFovDegrees * 0.5f));
	if (ForwardDot >= ForwardThreshold)
	{
		return EEnemyTurnDirection::None;
	}

	if (ForwardDot >= 0.f)
	{
		return RightDot >= 0.f ? EEnemyTurnDirection::Right : EEnemyTurnDirection::Left;
	}

	return RightDot >= 0.f ? EEnemyTurnDirection::OverRight : EEnemyTurnDirection::OverLeft;
}

bool US1EnemyLocomotionComponent::IsTargetForward(AActor* Target, float ForwardFovDegrees) const
{
	return ClassifyTurnDirection(Target, ForwardFovDegrees) == EEnemyTurnDirection::None;
}

void US1EnemyLocomotionComponent::TickApproach(float DeltaTime)
{
	AActor* Target = ApproachTarget.Get();
	AAIController* AIController = GetMonsterAIController();
	AS1Monster* Monster = GetMonsterOwner();
	US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance();
	UCharacterMovementComponent* Movement = GetMonsterMovement();
	if (Target == nullptr || AIController == nullptr || Monster == nullptr || AnimInstance == nullptr || Movement == nullptr)
	{
		AbortApproach(true);
		return;
	}
	const float DistanceToTarget = FVector::Dist2D(Monster->GetActorLocation(), Target->GetActorLocation());
	if (DistanceToTarget <= ChaseStopDistance)
	{
		BeginStop();
		return;
	}
	const float TargetSpeed = GetTargetMoveSpeed();
	if (AnimInstance->bLocomotionLoop || AnimInstance->LocomotionPhase == EEnemyLocomotionPhase::Loop)
	{
		Movement->MaxWalkSpeed = TargetSpeed;
	}
	else
	{
		Movement->MaxWalkSpeed = FMath::FInterpTo(Movement->MaxWalkSpeed, TargetSpeed, DeltaTime, AccelerationInterpSpeed);
	}
	if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
	{
		if (PathFollowingComponent->GetStatus() != EPathFollowingStatus::Moving)
		{
			AIController->MoveToActor(Target, MoveAcceptRadius);
		}
	}
}

void US1EnemyLocomotionComponent::BeginStop()
{
	if (ApproachState != EEnemyApproachState::Approaching)
	{
		return;
	}
	ApproachState = EEnemyApproachState::Stopping;
	if (AAIController* AIController = GetMonsterAIController())
	{
		AIController->StopMovement();
	}
	if (US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance())
	{
		AnimInstance->RequestStop();
		SyncReplicatedLocomotionState();
	}
}

void US1EnemyLocomotionComponent::RestoreMovementSpeed()
{
	if (UCharacterMovementComponent* Movement = GetMonsterMovement())
	{
		if (CachedMaxWalkSpeed > 0.f)
		{
			Movement->MaxWalkSpeed = CachedMaxWalkSpeed;
		}
	}
	CachedMaxWalkSpeed = 0.f;
}

AS1Monster* US1EnemyLocomotionComponent::GetMonsterOwner() const
{
	return Cast<AS1Monster>(GetOwner());
}

AAIController* US1EnemyLocomotionComponent::GetMonsterAIController() const
{
	if (AS1Monster* Monster = GetMonsterOwner())
	{
		return Cast<AAIController>(Monster->GetController());
	}
	return nullptr;
}

UCharacterMovementComponent* US1EnemyLocomotionComponent::GetMonsterMovement() const
{
	if (AS1Monster* Monster = GetMonsterOwner())
	{
		return Monster->GetCharacterMovement();
	}
	return nullptr;
}

US1AnimInstance_EnemyLocomotion* US1EnemyLocomotionComponent::GetEnemyAnimInstance() const
{
	if (AS1Monster* Monster = GetMonsterOwner())
	{
		if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
		{
			return Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance());
		}
	}
	return nullptr;
}

float US1EnemyLocomotionComponent::GetTargetMoveSpeed() const
{
	return LocomotionMode == EEnemyLocomotionMode::Walk ? WalkSpeed : RunSpeed;
}

bool US1EnemyLocomotionComponent::IsMoveLocomotionMode(EEnemyLocomotionMode Mode) const
{
	return Mode == EEnemyLocomotionMode::Walk || Mode == EEnemyLocomotionMode::Run;
}

void US1EnemyLocomotionComponent::SyncReplicatedLocomotionState()
{
	AS1Monster* Monster = GetMonsterOwner();
	if (Monster == nullptr || false == Monster->HasAuthority())
	{
		return;
	}

	if (US1AnimInstance_EnemyLocomotion* AnimInstance = GetEnemyAnimInstance())
	{
		Monster->SetReplicatedLocomotionState(AnimInstance->LocomotionMode, AnimInstance->LocomotionPhase, AnimInstance->bLocomotionLoop);
	}
}
