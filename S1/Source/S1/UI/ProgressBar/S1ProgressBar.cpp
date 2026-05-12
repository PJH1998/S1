// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProgressBar/S1ProgressBar.h"

US1ProgressBar::US1ProgressBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1ProgressBar::SetCurrentValue(float InValue)
{
	CurrentValue = InValue;
}

void US1ProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void US1ProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CurrentValue >= LerpValue)
	{
		LerpValue = CurrentValue;
		return;
	}

	LerpValue -= LerpSpeed * InDeltaTime;
}
