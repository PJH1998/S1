// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Task_Turn.generated.h"

class UAnimMontage;

UCLASS()
class S1_API UBT_Task_Turn : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_Turn();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void SetIsTurning(UBehaviorTreeComponent& OwnerComp, bool bIsInTurning) const;
	FName GetTurnSectionForDirection(EEnemyTurnDirection Direction) const;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTurningKey;

	UPROPERTY(EditAnywhere, Category = "Turn|Animation")
	TObjectPtr<UAnimMontage> TurnMontage;

	UPROPERTY(EditAnywhere, Category = "Turn|Animation")
	FName TurnSection_Left = "Left";

	UPROPERTY(EditAnywhere, Category = "Turn|Animation")
	FName TurnSection_Right = "Right";

	UPROPERTY(EditAnywhere, Category = "Turn|Animation")
	FName TurnSection_OverLeft = "OverLeft";

	UPROPERTY(EditAnywhere, Category = "Turn|Animation")
	FName TurnSection_OverRight = "OverRight";

	UPROPERTY(EditAnywhere, Category = "Turn", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float TurnForwardFov = 45.f;

	UPROPERTY(EditAnywhere, Category = "Turn", meta = (ClampMin = "0.0"))
	float MinTurnTimeBeforeForwardCheck = 0.15f;

	bool bTaskFinished = false;
	bool bTurnMontagePlaying = false;
	float TurnElapsedTime = 0.f;
};