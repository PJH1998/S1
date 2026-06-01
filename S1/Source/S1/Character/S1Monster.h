// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "S1Monster.generated.h"

class UBehaviorTree;
class UAnimMontage;

UCLASS()
class S1_API AS1Monster : public AS1Character
{
	GENERATED_BODY()
	
public:
	AS1Monster();

protected:
	virtual void BeginPlay() override;

public:
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	void PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void NotifyDeath();
	void HandleDeath();
	bool IsDead() const { return bIsDead; }

public:
	virtual void Tick(float DeltaTime) override;
	virtual void InitSystem() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

private:
	bool bIsDead = false;
};

