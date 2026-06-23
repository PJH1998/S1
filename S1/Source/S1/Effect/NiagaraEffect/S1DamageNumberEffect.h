// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "S1DamageNumberEffect.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class S1_API AS1DamageNumberEffect : public AS1NiagaraEffect
{
	GENERATED_BODY()

public:
	UNiagaraComponent* SpawnDigitComponent(UWorld* World, int32 DigitIndex) const;

	float GetRandomRadius() const { return RandomRadius; }
	float GetDigitSpacing() const { return DigitSpacing; }
	float GetLifetime() const { return Lifetime; }
	float GetRiseSpeed() const { return RiseSpeed; }
	FName GetDigitIndexParameterName() const { return DigitIndexParameterName; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float RandomRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float DigitSpacing = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float Lifetime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float RiseSpeed = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	FName DigitIndexParameterName = TEXT("User.DigitIndex");
};
