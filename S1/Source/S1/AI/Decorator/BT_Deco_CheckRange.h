// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BT_Deco_CheckRange.generated.h"

UENUM(BlueprintType)
enum class EBTCheckRangeCompare : uint8
{
	LessThan,
	GreaterOrEqual,
};

UCLASS()
class S1_API UBT_Deco_CheckRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBT_Deco_CheckRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float AttackRange = { 600.f };

	UPROPERTY(EditAnywhere, Category = "Range")
	EBTCheckRangeCompare Compare = EBTCheckRangeCompare::LessThan;
};
