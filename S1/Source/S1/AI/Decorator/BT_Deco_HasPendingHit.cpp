// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorator/BT_Deco_HasPendingHit.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"

UBT_Deco_HasPendingHit::UBT_Deco_HasPendingHit()
{
	NodeName = TEXT("Has Pending Hit");
}

bool UBT_Deco_HasPendingHit::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComponent)
	{
		return false;
	}

	if (IsDeadKey.SelectedKeyName != NAME_None && BlackboardComponent->GetValueAsBool(IsDeadKey.SelectedKeyName))
	{
		return false;
	}

	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn()))
		{
			if (Monster->IsDead())
			{
				return false;
			}
		}
	}

	if (PendingHitReactKey.SelectedKeyName == NAME_None)
	{
		return false;
	}

	return BlackboardComponent->GetValueAsBool(PendingHitReactKey.SelectedKeyName);
}
