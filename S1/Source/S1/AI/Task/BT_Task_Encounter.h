// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BT_Task_Encounter.generated.h"

class US1AbilitySystemComponent;
struct FAbilityEndedData;

/**
 * 최초 조우(인트로) 전용 Task. AbilityTag로 인트로 어빌리티를 발동하고 종료를 기다린 뒤,
 * 정상 종료 시에만 FlagKey(Bool) 블랙보드 값을 true로 세팅한다. "재생 끝 -> 플래그 set"을
 * 한 노드로 원자화하여, 어빌리티가 취소되면 플래그를 건드리지 않고 실패시켜 재시도를 보장한다.
 * 보스 전용이 아니라 일반 몬스터도 자신의 AbilityTag만 지정해 재사용할 수 있다.
 */
UCLASS()
class S1_API UBT_Task_Encounter : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_Encounter();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
	void ClearAbilityEndedDelegate();

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

	// 인트로 정상 종료 시 true로 세팅할 Bool 블랙보드 키.
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FlagKey;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY()
	TObjectPtr<US1AbilitySystemComponent> CachedAbilitySystemComponent;

	FDelegateHandle AbilityEndedDelegateHandle;
};
