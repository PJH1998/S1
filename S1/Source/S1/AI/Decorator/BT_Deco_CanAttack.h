// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BT_Deco_CanAttack.generated.h"

/**
 * 
 */
UCLASS()
class S1_API UBT_Deco_CanAttack : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBT_Deco_CanAttack();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector TargetKey;
};
