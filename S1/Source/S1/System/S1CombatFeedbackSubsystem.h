// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1CombatFeedbackSubsystem.generated.h"

class UCanvasPanel;
class US1DamageNumberWidget;

USTRUCT()
struct FS1ActiveFeedbackNumber
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<US1DamageNumberWidget> Widget = nullptr;

	FVector WorldLocation = FVector::ZeroVector;
	float ElapsedTime = 0.f;
	float Lifetime = 1.f;
	float RiseSpeed = 80.f;
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
	void ShowBlockNumber(const FVector& HitLocation);

private:
	static void SplitDamageIntoDigits(int32 Damage, TArray<int32>& OutDigits);

	FVector GetRandomizedLocation(const FVector& HitLocation) const;
	UCanvasPanel* GetOrResolveDamageNumberCanvas();
	US1DamageNumberWidget* AcquirePooledWidget();
	void ReleaseWidget(US1DamageNumberWidget* Widget);
	void RegisterActiveNumber(US1DamageNumberWidget* Widget, const FVector& HitLocation);
	void UpdateActiveNumber(FS1ActiveFeedbackNumber& ActiveNumber) const;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<US1DamageNumberWidget>> PooledWidgets;

	UPROPERTY(Transient)
	TArray<FS1ActiveFeedbackNumber> ActiveNumbers;

	TWeakObjectPtr<UCanvasPanel> CachedDamageNumberCanvas;

private:
	float RandomRadius = 50.f;
	float Lifetime = 1.f;
	float RiseSpeed = 50.f;
};
