// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1CombatFeedbackSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1CombatFeedbackSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void ShowDamageNumber(int32 Damage, const FVector& HitLocation) const;
};
