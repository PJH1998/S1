// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Task_Death.generated.h"

class UAnimMontage;
class UAnimSequence;
class UBehaviorTreeComponent;

UCLASS()
class S1_API UBT_Task_Death : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_Death();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnDeathAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void ClearMontageEndedDelegate();
	void SetBlackboardBool(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& KeySelector, bool bValue) const;

private:
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimSequence> DeathAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	FName AnimationSlotName = TEXT("DefaultSlot");

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (ClampMin = "0.0"))
	float BlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (ClampMin = "0.0"))
	float BlendOutTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsChasingKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTurningKey;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveDynamicMontage;

	FOnMontageEnded DeathMontageEndedDelegate;
};
