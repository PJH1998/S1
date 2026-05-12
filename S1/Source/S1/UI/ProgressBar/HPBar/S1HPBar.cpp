// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProgressBar/HPBar/S1HPBar.h"

US1HPBar::US1HPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1HPBar::NativeConstruct()
{
	Super::NativeConstruct();

	LerpValue = CurrentValue;
	LerpSpeed = 25.f;
}

void US1HPBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CurrentValue >= LerpValue)
	{
		LerpValue = CurrentValue;
	}
	else
	{
		LerpValue = FMath::Max(CurrentValue, LerpValue - LerpSpeed * InDeltaTime);
	}

	Bind_ShaderResource();
}

void US1HPBar::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetFillRatio(CurrentValue));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetFillRatio(LerpValue));
}
