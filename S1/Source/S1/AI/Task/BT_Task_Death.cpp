// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_Death.h"
#include "AI/S1AIController.h"
#include "Character/S1Monster.h"

UBT_Task_Death::UBT_Task_Death()
{
	NodeName = TEXT("Death");
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

EBTNodeResult::Type UBT_Task_Death::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AS1AIController* AIController = Cast<AS1AIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	if (!Monster || !Monster->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}