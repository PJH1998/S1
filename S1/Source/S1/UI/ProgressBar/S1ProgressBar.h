// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1ProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1ProgressBar : public US1BaseWidget
{
	GENERATED_BODY()
	
public:
	US1ProgressBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetCurrentValue(float InValue);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float CurrentValue = {};
	float LerpValue = {};
	float MaxValue = {};

	float LerpSpeed = { 0.5f };
};
