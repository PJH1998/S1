// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BT_Task_Chase.h"

#include "AIController.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Boss/S1BossBase.h"
#include "Component/S1BossLocomotionComponent.h"
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
	AS1BossBase* Boss = Cast<AS1BossBase>(Pawn);
	if (Boss == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent();
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

	AS1BossBase* Boss = Cast<AS1BossBase>(AIController->GetPawn());
	US1BossLocomotionComponent* LocomotionComponent = Boss ? Boss->GetLocomotionComponent() : nullptr;
	if (LocomotionComponent == nullptr)
	{
		bTaskFinished = true;
		SetIsChasing(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (LocomotionComponent->GetApproachState() == EBossApproachState::Stopping)
	{
		if (USkeletalMeshComponent* Mesh = Boss->GetMesh())
		{
			if (US1AnimInstance_BossBase* AnimInstance = Cast<US1AnimInstance_BossBase>(Mesh->GetAnimInstance()))
			{
				if (AnimInstance->LocomotionMode == EBossLocomotionMode::None)
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

	if (LocomotionComponent->GetApproachState() == EBossApproachState::Idle)
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
		if (AS1BossBase* Boss = Cast<AS1BossBase>(AIController->GetPawn()))
		{
			if (US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent())
			{
				LocomotionComponent->AbortApproach(true);
			}
		}
	}

	SetIsChasing(OwnerComp, false);
	bTaskFinished = true;
	return EBTNodeResult::Aborted;
}
