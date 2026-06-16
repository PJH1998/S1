// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Task_BackJump.generated.h"

class UAnimMontage;

UCLASS()
class S1_API UBT_Task_BackJump : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_BackJump();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void ClearCachedState();

private:
	UPROPERTY(EditAnywhere, Category = "BackJump|Animation")
	TObjectPtr<UAnimMontage> BackJumpMontage;

	UPROPERTY(EditAnywhere, Category = "BackJump|Animation")
	FName StartSection = NAME_None;

	UPROPERTY(EditAnywhere, Category = "BackJump", meta = (ClampMin = "0.0"))
	float BackwardMovementInputScale = 1.f;

	UPROPERTY(EditAnywhere, Category = "BackJump")
	bool bStopMontageOnAbort = true;

	UPROPERTY()
	TObjectPtr<class AS1Monster> CachedMonster;

	bool bTaskFinished = false;
};
