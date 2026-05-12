// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProgressBar/S1ProgressBar.h"

#include "Components/Image.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

US1ProgressBar::US1ProgressBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1ProgressBar::SetCurrentValue(float InValue)
{
	CurrentValue = FMath::Clamp(InValue, 0.f, MaxValue);
}

void US1ProgressBar::SetMaxValue(float InValue)
{
	MaxValue = FMath::Max(InValue, 0.f);
	CurrentValue = FMath::Clamp(CurrentValue, 0.f, MaxValue);
}

void US1ProgressBar::SetValue(float InCurrentValue, float InMaxValue)
{
	SetMaxValue(InMaxValue);
	SetCurrentValue(InCurrentValue);
}

void US1ProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeFillMaterial();
}

void US1ProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void US1ProgressBar::InitializeFillMaterial()
{
	if (Image_Fill == nullptr)
	{
		return;
	}

	Fill_MID = UMaterialInstanceDynamic::Create(GaugeMaterial, this);
	if (Fill_MID == nullptr)
	{
		UE_LOG(LogWindows, Error, TEXT("Failed to Create : Fill MID"));
		return;
	}
	Image_Fill->SetBrushFromMaterial(Fill_MID);
}

float US1ProgressBar::GetFillRatio(float InValue) const
{
	if (MaxValue <= KINDA_SMALL_NUMBER)
	{
		return 0.f;
	}

	return FMath::Clamp(InValue / MaxValue, 0.f, 1.f);
}
