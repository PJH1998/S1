// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BT_Deco_CheckRange.h"
#include "Character/Player/S1Player.h"
#include "AI/S1AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBT_Deco_CheckRange::UBT_Deco_CheckRange()
{
}

bool UBT_Deco_CheckRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllerPawn == nullptr)
	{
		return false;
	}

	AS1Player* Target = Cast<AS1Player>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	return Target->GetDistanceTo(ControllerPawn) < AttackRange;
}
