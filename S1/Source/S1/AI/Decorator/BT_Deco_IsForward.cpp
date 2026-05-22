// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BT_Deco_IsForward.h"
#include "BT_Deco_IsForward.h"
#include "AI/S1AIController.h"

#include "BehaviorTree/BlackboardComponent.h"

UBT_Deco_IsForward::UBT_Deco_IsForward()
{
}

bool UBT_Deco_IsForward::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();
	if (Target == nullptr || Owner == nullptr)
	{
		return false;
	}

	FVector Direction = Target->GetActorLocation() - Owner->GetActorLocation();
	Direction.Z = 0.f;
	if (false == Direction.Normalize())
	{
		return false;
	}

	FVector Forward = Owner->GetActorForwardVector();
	Forward.Z = 0.f;
	if (false == Forward.Normalize())
	{
		return false;
	}

	const float Dot = FVector::DotProduct(Forward, Direction);
	const float HalfFovRadians = FMath::DegreesToRadians(Fov * 0.5f);
	const float FovThreshold = FMath::Cos(HalfFovRadians);

	return Dot >= FovThreshold;
}
