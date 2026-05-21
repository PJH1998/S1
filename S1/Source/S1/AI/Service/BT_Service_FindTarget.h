// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BT_Service_FindTarget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class S1_API UBT_Service_FindTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBT_Service_FindTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SearchRadius = { 1000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector TargetKey;
};
