// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Monster.h"
#include "S1BossBase.generated.h"

class US1BossSet;

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
	virtual ES1EnemyTier GetLockOnTier_Implementation() override;

public:
	US1BossSet* GetS1BossSet() const;
	const FName& GetBossName() const;

protected:
	FName BossName = {};
};
