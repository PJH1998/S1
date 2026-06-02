// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_PlayAbility.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"

UBT_Task_PlayAbility::UBT_Task_PlayAbility()
{
	NodeName = TEXT("Play Ability");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_Task_PlayAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (false == AbilityTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (nullptr == AIController)
	{
		return EBTNodeResult::Failed;
	}
	APawn* Pawn = AIController->GetPawn();
	if (AS1Monster* Monster = Cast<AS1Monster>(Pawn))
	{
		if (Monster->IsDead())
		{
			return EBTNodeResult::Failed;
		}
	}
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (nullptr == AbilitySystemInterface)
	{
		return EBTNodeResult::Failed;
	}
	US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (nullptr == ASC)
	{
		return EBTNodeResult::Failed;
	}
	CachedOwnerComp = &OwnerComp;
	CachedAbilitySystemComponent = ASC;
	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBT_Task_PlayAbility::OnAbilityEnded);
	if (ASC->ActivateAbility(AbilityTag) == false)
	{
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBT_Task_PlayAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ClearAbilityEndedDelegate();
	return EBTNodeResult::Aborted;
}

void UBT_Task_PlayAbility::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;
	ClearAbilityEndedDelegate();
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, AbilityEndedData.bWasCancelled ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
	}
}

void UBT_Task_PlayAbility::ClearAbilityEndedDelegate()
{
	if (CachedAbilitySystemComponent && AbilityEndedDelegateHandle.IsValid())
	{
		CachedAbilitySystemComponent->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	AbilityEndedDelegateHandle.Reset();
	CachedOwnerComp = nullptr;
	CachedAbilitySystemComponent = nullptr;
}
