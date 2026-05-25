// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Monster.h"
#include "S1BossBase.generated.h"

class AS1BossBase;
class US1BossSet;

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

public:
	void NotifyHasTargetChanged(bool bInHasTarget);
	US1BossSet* GetS1BossSet() const;
	const FName& GetBossName() const;

public:
	UPROPERTY(BlueprintAssignable)
	FBossHasTargetChangedDelegate OnHasTargetChanged;

protected:
	FName BossName = {};

private:
	bool bHasTarget = { false };
};
