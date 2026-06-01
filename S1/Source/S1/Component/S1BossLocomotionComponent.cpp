// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1BossLocomotionComponent.h"

#include "AIController.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "Character/Boss/S1BossBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

US1BossLocomotionComponent::US1BossLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void US1BossLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void US1BossLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ApproachState == EBossApproachState::Approaching)
	{
		TickApproach(DeltaTime);
	}
}

bool US1BossLocomotionComponent::StartApproach(AActor* Target, EBossLocomotionMode Mode)
{
	if (Target == nullptr || IsMoveLocomotionMode(Mode) == false)
	{
		return false;
	}

	if (TurnState == EBossTurnState::Turning)
	{
		AbortTurn(true);
	}
	else if (TurnState == EBossTurnState::TurnComplete)
	{
		TurnState = EBossTurnState::Idle;
	}

	if (ApproachState == EBossApproachState::Approaching || ApproachState == EBossApproachState::Stopping)
	{
		AbortApproach(true);
	}

	AS1BossBase* Boss = GetBossOwner();
	AAIController* AIController = GetBossAIController();
	US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance();
	UCharacterMovementComponent* Movement = GetBossMovement();
	if (Boss == nullptr || AIController == nullptr || AnimInstance == nullptr || Movement == nullptr)
	{
		return false;
	}

	ApproachTarget = Target;
	LocomotionMode = Mode;
	ApproachState = EBossApproachState::Approaching;

	CachedMaxWalkSpeed = Movement->MaxWalkSpeed;
	Movement->MaxWalkSpeed = FMath::Max(50.f, GetTargetMoveSpeed() * 0.15f);

	AnimInstance->BeginApproach(Mode);

	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToActor(Target, MoveAcceptRadius);
	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		AbortApproach(true);
		return false;
	}

	SetComponentTickEnabled(true);
	return true;
}

void US1BossLocomotionComponent::AbortApproach(bool bInstantReset)
{
	AAIController* AIController = GetBossAIController();
	if (AIController)
	{
		AIController->StopMovement();
	}

	RestoreMovementSpeed();

	if (US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance())
	{
		if (bInstantReset)
		{
			AnimInstance->ResetLocomotion();
		}
		else if (AnimInstance->LocomotionMode != EBossLocomotionMode::None)
		{
			AnimInstance->RequestStop();
		}
	}

	ApproachTarget = nullptr;
	ApproachState = EBossApproachState::Idle;

	if (TurnState == EBossTurnState::Idle)
	{
		SetComponentTickEnabled(false);
	}
}

void US1BossLocomotionComponent::NotifyLocomotionStopFinished()
{
	if (ApproachState != EBossApproachState::Stopping)
	{
		return;
	}

	ApproachState = EBossApproachState::StopComplete;
	ApproachTarget = nullptr;
	RestoreMovementSpeed();

	if (TurnState == EBossTurnState::Idle)
	{
		SetComponentTickEnabled(false);
	}
}

bool US1BossLocomotionComponent::StartTurn(AActor* Target)
{
	if (Target == nullptr)
	{
		return false;
	}

	const EBossTurnDirection TurnDirection = ClassifyTurnDirection(Target);
	if (TurnDirection == EBossTurnDirection::None)
	{
		return false;
	}

	if (ApproachState == EBossApproachState::Approaching || ApproachState == EBossApproachState::Stopping)
	{
		AbortApproach(true);
	}
	else if (ApproachState == EBossApproachState::StopComplete)
	{
		ApproachState = EBossApproachState::Idle;
	}

	if (TurnState == EBossTurnState::Turning)
	{
		AbortTurn(true);
	}

	if (TurnState == EBossTurnState::TurnComplete)
	{
		TurnState = EBossTurnState::Idle;
	}

	AS1BossBase* Boss = GetBossOwner();
	AAIController* AIController = GetBossAIController();
	US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance();
	if (Boss == nullptr || AIController == nullptr || AnimInstance == nullptr)
	{
		return false;
	}

	AIController->StopMovement();

	CurrentTurnDirection = TurnDirection;
	TurnState = EBossTurnState::Turning;

	AnimInstance->BeginTurn(TurnDirection);

	return true;
}

void US1BossLocomotionComponent::AbortTurn(bool bInstantReset)
{
	if (AAIController* AIController = GetBossAIController())
	{
		AIController->StopMovement();
	}

	if (US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance())
	{
		if (bInstantReset)
		{
			AnimInstance->ResetLocomotion();
		}
	}

	CurrentTurnDirection = EBossTurnDirection::None;
	TurnState = EBossTurnState::Idle;

	if (ApproachState == EBossApproachState::Idle || ApproachState == EBossApproachState::StopComplete)
	{
		SetComponentTickEnabled(false);
	}
}

void US1BossLocomotionComponent::NotifyTurnFinished()
{
	if (TurnState != EBossTurnState::Turning)
	{
		return;
	}

	TurnState = EBossTurnState::TurnComplete;
	CurrentTurnDirection = EBossTurnDirection::None;
}

bool US1BossLocomotionComponent::IsTargetForward(AActor* Target) const
{
	return ClassifyTurnDirection(Target) == EBossTurnDirection::None;
}

EBossTurnDirection US1BossLocomotionComponent::ClassifyTurnDirection(AActor* Target) const
{
	AS1BossBase* Boss = GetBossOwner();
	if (Boss == nullptr || Target == nullptr)
	{
		return EBossTurnDirection::None;
	}

	FVector ToTarget = Target->GetActorLocation() - Boss->GetActorLocation();
	ToTarget.Z = 0.f;
	if (ToTarget.Normalize() == false)
	{
		return EBossTurnDirection::None;
	}

	FVector Forward = Boss->GetActorForwardVector();
	Forward.Z = 0.f;
	if (Forward.Normalize() == false)
	{
		return EBossTurnDirection::None;
	}

	const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);
	const float ForwardDot = FVector::DotProduct(Forward, ToTarget);
	const float RightDot = FVector::DotProduct(Right, ToTarget);
	const float ForwardThreshold = GetForwardAngleThresholdCos();

	if (ForwardDot >= ForwardThreshold)
	{
		return EBossTurnDirection::None;
	}

	if (ForwardDot >= 0.f)
	{
		return RightDot >= 0.f ? EBossTurnDirection::Right : EBossTurnDirection::Left;
	}

	return RightDot >= 0.f ? EBossTurnDirection::OverRight : EBossTurnDirection::OverLeft;
}

void US1BossLocomotionComponent::TickApproach(float DeltaTime)
{
	AActor* Target = ApproachTarget.Get();
	AAIController* AIController = GetBossAIController();
	AS1BossBase* Boss = GetBossOwner();
	US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance();
	UCharacterMovementComponent* Movement = GetBossMovement();
	if (Target == nullptr || AIController == nullptr || Boss == nullptr || AnimInstance == nullptr || Movement == nullptr)
	{
		AbortApproach(true);
		return;
	}

	const float DistanceToTarget = FVector::Dist2D(Boss->GetActorLocation(), Target->GetActorLocation());
	if (DistanceToTarget <= ChaseStopDistance)
	{
		BeginStop();
		return;
	}

	const float TargetSpeed = GetTargetMoveSpeed();
	if (AnimInstance->bLocomotionLoop || AnimInstance->LocomotionPhase == EBossLocomotionPhase::Loop)
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

void US1BossLocomotionComponent::BeginStop()
{
	if (ApproachState != EBossApproachState::Approaching)
	{
		return;
	}

	ApproachState = EBossApproachState::Stopping;

	if (AAIController* AIController = GetBossAIController())
	{
		AIController->StopMovement();
	}

	if (US1AnimInstance_BossBase* AnimInstance = GetBossAnimInstance())
	{
		AnimInstance->RequestStop();
	}
}

void US1BossLocomotionComponent::RestoreMovementSpeed()
{
	if (UCharacterMovementComponent* Movement = GetBossMovement())
	{
		if (CachedMaxWalkSpeed > 0.f)
		{
			Movement->MaxWalkSpeed = CachedMaxWalkSpeed;
		}
	}

	CachedMaxWalkSpeed = 0.f;
}

float US1BossLocomotionComponent::GetForwardAngleThresholdCos() const
{
	return FMath::Cos(FMath::DegreesToRadians(TurnForwardFov * 0.5f));
}

AS1BossBase* US1BossLocomotionComponent::GetBossOwner() const
{
	return Cast<AS1BossBase>(GetOwner());
}

AAIController* US1BossLocomotionComponent::GetBossAIController() const
{
	if (AS1BossBase* Boss = GetBossOwner())
	{
		return Cast<AAIController>(Boss->GetController());
	}

	return nullptr;
}

UCharacterMovementComponent* US1BossLocomotionComponent::GetBossMovement() const
{
	if (AS1BossBase* Boss = GetBossOwner())
	{
		return Boss->GetCharacterMovement();
	}

	return nullptr;
}

US1AnimInstance_BossBase* US1BossLocomotionComponent::GetBossAnimInstance() const
{
	if (AS1BossBase* Boss = GetBossOwner())
	{
		if (USkeletalMeshComponent* Mesh = Boss->GetMesh())
		{
			return Cast<US1AnimInstance_BossBase>(Mesh->GetAnimInstance());
		}
	}

	return nullptr;
}

float US1BossLocomotionComponent::GetTargetMoveSpeed() const
{
	return LocomotionMode == EBossLocomotionMode::Walk ? WalkSpeed : RunSpeed;
}

bool US1BossLocomotionComponent::IsMoveLocomotionMode(EBossLocomotionMode Mode) const
{
	return Mode == EBossLocomotionMode::Walk || Mode == EBossLocomotionMode::Run;
}
