// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_Chase.h"
#include "AIController.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_Chase::UBT_Task_Chase()
{
	NodeName = TEXT("Chase");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

void UBT_Task_Chase::SetIsChasing(UBehaviorTreeComponent& OwnerComp, bool bIsChasing) const
{
	if (IsChasingKey.SelectedKeyName == NAME_None)
	{
		return;
	}
	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(IsChasingKey.SelectedKeyName, bIsChasing);
	}
}

EBTNodeResult::Type UBT_Task_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	SetIsChasing(OwnerComp, false);
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	APawn* Pawn = AIController->GetPawn();
	AS1Monster* Monster = Cast<AS1Monster>(Pawn);
	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	if (Monster->IsDead())
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
	if (LocomotionComponent->StartApproach(Target, LocomotionMode) == false)
	{
		return EBTNodeResult::Failed;
	}
	SetIsChasing(OwnerComp, true);
	return EBTNodeResult::InProgress;
}

void UBT_Task_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (bTaskFinished)
	{
		return;
	}
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		bTaskFinished = true;
		SetIsChasing(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	US1EnemyLocomotionComponent* LocomotionComponent = Monster ? Monster->GetLocomotionComponent() : nullptr;
	if (LocomotionComponent == nullptr)
	{
		bTaskFinished = true;
		SetIsChasing(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	if (LocomotionComponent->GetApproachState() == EEnemyApproachState::Stopping)
	{
		if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
		{
			if (US1AnimInstance_EnemyLocomotion* AnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()))
			{
				if (AnimInstance->LocomotionMode == EEnemyLocomotionMode::None)
				{
					LocomotionComponent->NotifyLocomotionStopFinished();
				}
			}
		}
	}
	if (LocomotionComponent->IsApproachComplete())
	{
		bTaskFinished = true;
		SetIsChasing(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	if (LocomotionComponent->GetApproachState() == EEnemyApproachState::Idle)
	{
		bTaskFinished = true;
		SetIsChasing(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBT_Task_Chase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn()))
		{
			if (US1EnemyLocomotionComponent* LocomotionComponent = Monster->GetLocomotionComponent())
			{
				LocomotionComponent->AbortApproach(true);
			}
		}
	}
	SetIsChasing(OwnerComp, false);
	bTaskFinished = true;
	return EBTNodeResult::Aborted;
}
