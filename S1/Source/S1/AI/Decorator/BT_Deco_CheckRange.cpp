// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BT_Deco_CheckRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBT_Deco_CheckRange::UBT_Deco_CheckRange()
{
}

bool UBT_Deco_CheckRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllerPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (ControllerPawn == nullptr)
	{
		return false;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return false;
	}

	AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	const float Distance = Target->GetDistanceTo(ControllerPawn);
	if (Compare == EBTCheckRangeCompare::GreaterOrEqual)
	{
		return Distance >= AttackRange;
	}

	return Distance < AttackRange;
}
