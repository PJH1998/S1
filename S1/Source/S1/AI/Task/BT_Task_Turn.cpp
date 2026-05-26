// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BT_Task_Turn.h"

#include "AIController.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Boss/S1BossBase.h"
#include "Component/S1BossLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_Turn::UBT_Task_Turn()
{
	NodeName = TEXT("Turn");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

void UBT_Task_Turn::SetIsTurning(UBehaviorTreeComponent& OwnerComp, bool bIsTurning) const
{
	if (IsTurningKey.SelectedKeyName == NAME_None)
	{
		return;
	}

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(IsTurningKey.SelectedKeyName, bIsTurning);
	}
}

EBTNodeResult::Type UBT_Task_Turn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	SetIsTurning(OwnerComp, false);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AS1BossBase* Boss = Cast<AS1BossBase>(AIController->GetPawn());
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

	if (LocomotionComponent->StartTurn(Target) == false)
	{
		return EBTNodeResult::Failed;
	}

	SetIsTurning(OwnerComp, true);
	return EBTNodeResult::InProgress;
}

void UBT_Task_Turn::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AS1BossBase* Boss = Cast<AS1BossBase>(AIController->GetPawn());
	US1BossLocomotionComponent* LocomotionComponent = Boss ? Boss->GetLocomotionComponent() : nullptr;
	if (LocomotionComponent == nullptr)
	{
		bTaskFinished = true;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (LocomotionComponent->GetTurnState() == EBossTurnState::Turning)
	{
		if (USkeletalMeshComponent* Mesh = Boss->GetMesh())
		{
			if (US1AnimInstance_BossBase* AnimInstance = Cast<US1AnimInstance_BossBase>(Mesh->GetAnimInstance()))
			{
				if (AnimInstance->LocomotionMode == EBossLocomotionMode::None)
				{
					LocomotionComponent->NotifyTurnFinished();
				}
			}
		}
	}

	if (LocomotionComponent->IsTurnComplete())
	{
		bTaskFinished = true;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (LocomotionComponent->GetTurnState() == EBossTurnState::Idle)
	{
		bTaskFinished = true;
		SetIsTurning(OwnerComp, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBT_Task_Turn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (AS1BossBase* Boss = Cast<AS1BossBase>(AIController->GetPawn()))
		{
			if (US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent())
			{
				LocomotionComponent->AbortTurn(true);
			}
		}
	}

	SetIsTurning(OwnerComp, false);
	bTaskFinished = true;
	return EBTNodeResult::Aborted;
}
