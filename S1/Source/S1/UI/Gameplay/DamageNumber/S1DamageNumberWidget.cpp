// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/DamageNumber/S1DamageNumberWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void US1DamageNumberWidget::ShowDigits(const TArray<int32>& Digits)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (Image_Block)
	{
		Image_Block->SetVisibility(ESlateVisibility::Collapsed);
	}

	const TArray<UImage*> DigitImages = GetDigitImages();
	for (int32 Index = 0; Index < DigitImages.Num(); ++Index)
	{
		UImage* DigitImage = DigitImages[Index];
		if (DigitImage == nullptr)
		{
			continue;
		}

		if (Digits.IsValidIndex(Index) && DigitTextures.IsValidIndex(Digits[Index]) && DigitTextures[Digits[Index]] != nullptr)
		{
			DigitImage->SetBrushFromTexture(DigitTextures[Digits[Index]]);
			DigitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			DigitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void US1DamageNumberWidget::ShowBlock()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	for (UImage* DigitImage : GetDigitImages())
	{
		if (DigitImage)
		{
			DigitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (Image_Block)
	{
		Image_Block->SetBrushFromTexture(BlockTexture);
		Image_Block->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void US1DamageNumberWidget::ResetForPool()
{
	SetVisibility(ESlateVisibility::Collapsed);

	for (UImage* DigitImage : GetDigitImages())
	{
		if (DigitImage)
		{
			DigitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (Image_Block)
	{
		Image_Block->SetVisibility(ESlateVisibility::Collapsed);
	}
}

TArray<UImage*> US1DamageNumberWidget::GetDigitImages() const
{
	return { Image_Digit0, Image_Digit1, Image_Digit2, Image_Digit3, Image_Digit4 };
}
