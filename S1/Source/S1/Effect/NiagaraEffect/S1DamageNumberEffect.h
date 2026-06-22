// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "S1DamageNumberEffect.generated.h"

/**
 * 
 */
UCLASS()
class S1_API AS1DamageNumberEffect : public AS1NiagaraEffect
{
	GENERATED_BODY()

public:
	void PlayDamageNumber(UWorld* World, int32 Damage, const FVector& HitLocation) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float RandomRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float DigitSpacing = 18.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	FName DigitIndexParameterName = TEXT("User.DigitIndex");
};
