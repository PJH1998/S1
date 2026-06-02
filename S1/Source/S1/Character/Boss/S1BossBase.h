// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Monster.h"
#include "S1BossBase.generated.h"

class AS1BossBase;
class US1BossSet;
class US1BossLocomotionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBossHasTargetChangedDelegate, AS1BossBase*, Boss, bool, bHasTarget);

/**
 * 
 */
UCLASS()
class S1_API AS1BossBase : public AS1Monster
{
	GENERATED_BODY()
	
public:
	AS1BossBase();

	virtual void NotifyDeath() override;

public:
	void NotifyHasTargetChanged(bool bInHasTarget);
	US1BossSet* GetS1BossSet() const;
	const FName& GetBossName() const;
	US1BossLocomotionComponent* GetLocomotionComponent() const { return LocomotionComponent; }

public:
	UPROPERTY(BlueprintAssignable)
	FBossHasTargetChangedDelegate OnHasTargetChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
	TObjectPtr<US1BossLocomotionComponent> LocomotionComponent;

protected:
	FName BossName = {};

private:
	bool bHasTarget = { false };
};
