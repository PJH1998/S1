// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_Encounter.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/S1Monster.h"

UBT_Task_Encounter::UBT_Task_Encounter()
{
	NodeName = TEXT("Encounter");
	bCreateNodeInstance = true;

	FlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBT_Task_Encounter, FlagKey));
}

void UBT_Task_Encounter::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BlackboardAsset = GetBlackboardAsset())
	{
		FlagKey.ResolveSelectedKey(*BlackboardAsset);
	}
}

EBTNodeResult::Type UBT_Task_Encounter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	if (Pawn == nullptr || false == Pawn->HasAuthority())
	{
		return EBTNodeResult::Failed;
	}

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
	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBT_Task_Encounter::OnAbilityEnded);
	if (ASC->ActivateAbility(AbilityTag) == false)
	{
		ClearAbilityEndedDelegate();
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBT_Task_Encounter::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	US1AbilitySystemComponent* ASC = CachedAbilitySystemComponent;
	ClearAbilityEndedDelegate();
	if (ASC)
	{
		ASC->CancelAllAbilities();
	}
	return EBTNodeResult::Aborted;
}

void UBT_Task_Encounter::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;
	ClearAbilityEndedDelegate();
	if (OwnerComp == nullptr)
	{
		return;
	}

	const bool bSucceeded = (false == AbilityEndedData.bWasCancelled);
	if (bSucceeded)
	{
		if (UBlackboardComponent* BlackboardComponent = OwnerComp->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FlagKey.SelectedKeyName, true);
		}
	}

	FinishLatentTask(*OwnerComp, bSucceeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
}

void UBT_Task_Encounter::ClearAbilityEndedDelegate()
{
	if (CachedAbilitySystemComponent && AbilityEndedDelegateHandle.IsValid())
	{
		CachedAbilitySystemComponent->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	AbilityEndedDelegateHandle.Reset();
	CachedOwnerComp = nullptr;
	CachedAbilitySystemComponent = nullptr;
}