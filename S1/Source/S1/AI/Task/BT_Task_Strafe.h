// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Task_Strafe.generated.h"

class UAnimMontage;
class UAnimInstance;
class AActor;

UENUM(BlueprintType)
enum class ES1StrafeDirection : uint8
{
	Left,
	Right,
};

UCLASS()
class S1_API UBT_Task_Strafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_Strafe();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	float GetStrafeDirectionScalar() const;
	ES1StrafeDirection ClassifyStrafeDirection(class AS1NormalMonster* Monster, AActor* Target) const;
	UAnimMontage* GetStrafeMontageForDirection(ES1StrafeDirection Direction) const;
	bool IsStrafeLoopSection(const UAnimInstance* AnimInstance) const;
	void ClearCachedState();

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Strafe|Animation")
	TObjectPtr<UAnimMontage> LeftStrafeMontage;

	UPROPERTY(EditAnywhere, Category = "Strafe|Animation")
	TObjectPtr<UAnimMontage> RightStrafeMontage;

	UPROPERTY(EditAnywhere, Category = "Strafe|Animation")
	FName StartSection = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Strafe|Animation")
	FName LoopSection = FName("Loop");

	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "0.0"))
	float MovementInputScale = 1.f;

	UPROPERTY(EditAnywhere, Category = "Strafe")
	bool bStopMontageOnAbort = true;

	UPROPERTY()
	TObjectPtr<class AS1NormalMonster> CachedMonster;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveStrafeMontage;

	ES1StrafeDirection ActiveStrafeDirection = ES1StrafeDirection::Right;
	bool bTaskFinished = false;
};
