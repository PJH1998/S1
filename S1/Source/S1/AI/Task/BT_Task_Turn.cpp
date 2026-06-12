// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_Turn.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_Turn::UBT_Task_Turn()
{
	NodeName = TEXT("Turn");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

void UBT_Task_Turn::SetIsTurning(UBehaviorTreeComponent& OwnerComp, bool bIsInTurning) const
{
	if (IsTurningKey.SelectedKeyName == NAME_None)
	{
		return;
	}
	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(IsTurningKey.SelectedKeyName, bIsInTurning);
	}
}

FName UBT_Task_Turn::GetTurnSectionForDirection(EEnemyTurnDirection Direction) const
{
	switch (Direction)
	{
	case EEnemyTurnDirection::Left:
		return TurnSection_Left;
	case EEnemyTurnDirection::Right:
		return TurnSection_Right;
	case EEnemyTurnDirection::OverLeft:
		return TurnSection_OverLeft;
	case EEnemyTurnDirection::OverRight:
		return TurnSection_OverRight;
	default:
		return NAME_None;
	}
}

EBTNodeResult::Type UBT_Task_Turn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	bTurnMontagePlaying = false;
	TurnElapsedTime = 0.f;
	SetIsTurning(OwnerComp, false);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	US1EnemyLocomotionComponent* LocomotionComponent = Monster->GetLocomotionComponent();
	if (LocomotionComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const EEnemyTurnDirection TurnDirection = LocomotionComponent->ClassifyTurnDirection(Target, TurnForwardFov);
	if (TurnDirection == EEnemyTurnDirection::None)
	{
		return EBTNodeResult::Succeeded;
	}

	if (LocomotionComponent->GetApproachState() == EEnemyApproachState::Approaching
		|| LocomotionComponent->GetApproachState() == EEnemyApproachState::Stopping
		|| LocomotionComponent->GetApproachState() == EEnemyApproachState::StopComplete)
	{
		LocomotionComponent->AbortApproach(true);
	}

	USkeletalMeshComponent* Mesh = Monster->GetMesh();
	US1AnimInstance_EnemyLocomotion* AnimInstance = Mesh ? Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()) : nullptr;
	if (AnimInstance == nullptr || TurnMontage == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const FName TurnSection = GetTurnSectionForDirection(TurnDirection);
	if (TurnSection.IsNone())
	{
		return EBTNodeResult::Failed;
	}

	AIController->StopMovement();

	if (!AnimInstance->PlayTurnMontage(TurnMontage, TurnSection))
	{
		return EBTNodeResult::Failed;
	}

	bTurnMontagePlaying = true;
	SetIsTurning(OwnerComp, true);
	return EBTNodeResult::InProgress;
}

void UBT_Task_Turn::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (bTaskFinished || !bTurnMontagePlaying)
	{
		return;
	}

	TurnElapsedTime += DeltaSeconds;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		bTaskFinished = true;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	US1EnemyLocomotionComponent* LocomotionComponent = Monster ? Monster->GetLocomotionComponent() : nullptr;
	USkeletalMeshComponent* Mesh = Monster ? Monster->GetMesh() : nullptr;
	US1AnimInstance_EnemyLocomotion* AnimInstance = Mesh ? Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()) : nullptr;
	if (LocomotionComponent == nullptr || AnimInstance == nullptr)
	{
		bTaskFinished = true;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AActor* Target = BlackboardComponent
		? Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName))
		: nullptr;

	const bool bMontageFinished = !AnimInstance->IsPlayingTurnMontage();
	const bool bFacingTarget = Target != nullptr
		&& TurnElapsedTime >= MinTurnTimeBeforeForwardCheck
		&& LocomotionComponent->IsTargetForward(Target, TurnForwardFov);

	if (bMontageFinished)// || bFacingTarget)
	{
		AnimInstance->StopTurnMontage();
		bTaskFinished = true;
		bTurnMontagePlaying = false;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBT_Task_Turn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn()))
		{
			if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
			{
				if (US1AnimInstance_EnemyLocomotion* AnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()))
				{
					AnimInstance->StopTurnMontage();
				}
			}
		}
	}

	SetIsTurning(OwnerComp, false);
	bTaskFinished = true;
	bTurnMontagePlaying = false;
	return EBTNodeResult::Aborted;
}