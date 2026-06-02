// Fill out your copyright notice in the Description page of Project Settings.



#pragma once



#include "CoreMinimal.h"

#include "BehaviorTree/BTTaskNode.h"

#include "BT_Task_Death.generated.h"



/** 레거시 BT 노드. 데스 재생·연출은 ABP(bIsDeadAnim + AnimNotify)에서 처리. */

UCLASS()

class S1_API UBT_Task_Death : public UBTTaskNode

{

	GENERATED_BODY()



public:

	UBT_Task_Death();



protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};


