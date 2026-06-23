// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1CombatFeedbackSubsystem.generated.h"

class AS1DamageNumberEffect;
class UNiagaraComponent;

USTRUCT()
struct FS1DamageDigitInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> Component = nullptr;

	float OffsetFromCenter = 0.f;
};

USTRUCT()
struct FS1ActiveDamageNumberGroup
{
	GENERATED_BODY()

	FVector CenterWorldLocation = FVector::ZeroVector;
	float ElapsedTime = 0.f;
	float Lifetime = 1.f;
	float RiseSpeed = 80.f;
	float DigitSpacing = 90.f;

	UPROPERTY()
	TArray<FS1DamageDigitInstance> Digits;
};

/**
 * 
 */
UCLASS()
class S1_API US1CombatFeedbackSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	void ShowDamageNumber(int32 Damage, const FVector& HitLocation);

private:
	static void SplitDamageIntoDigits(int32 Damage, TArray<int32>& OutDigits);
	static bool GetCameraBasis(UWorld* World, FVector& OutRight, FRotator& OutRotation);
	void UpdateDamageNumberGroup(FS1ActiveDamageNumberGroup& Group) const;
	void FinishDamageNumberGroup(FS1ActiveDamageNumberGroup& Group);
	const AS1DamageNumberEffect* GetDamageNumberEffect() const;

private:
	UPROPERTY(Transient)
	TArray<FS1ActiveDamageNumberGroup> ActiveDamageNumberGroups;
};
