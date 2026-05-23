// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Monster.h"
#include "S1BossBase.generated.h"

class AS1BossBase;

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

public:
	UPROPERTY(BlueprintAssignable)
	FBossHasTargetChangedDelegate OnHasTargetChanged;

private:
	bool bHasTarget = { false };
};
