// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "S1Monster.generated.h"

class UBlackboardComponent;
class UBehaviorTree;
/**
 * 
 */
UCLASS()
class S1_API AS1Monster : public AS1Character
{
	GENERATED_BODY()
	
public:
	AS1Monster();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void InitSystem() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> Blackboard;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
