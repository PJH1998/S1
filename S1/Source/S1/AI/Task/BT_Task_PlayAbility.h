// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BT_Task_PlayAbility.generated.h"

class US1AbilitySystemComponent;
struct FAbilityEndedData;

/**
 * 
 */
UCLASS()
class S1_API UBT_Task_PlayAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_PlayAbility();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	void ClearAbilityEndedDelegate();

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY()
	TObjectPtr<US1AbilitySystemComponent> CachedAbilitySystemComponent;

	FDelegateHandle AbilityEndedDelegateHandle;
};
