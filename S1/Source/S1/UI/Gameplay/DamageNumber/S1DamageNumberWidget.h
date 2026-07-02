// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1DamageNumberWidget.generated.h"

class UHorizontalBox;
class UImage;
class UTexture2D;

/**
 *
 */
UCLASS()
class S1_API US1DamageNumberWidget : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void ShowDigits(const TArray<int32>& Digits);
	void ShowBlock();
	void ResetForPool();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Digits;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Digit0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Digit1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Digit2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Digit3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Digit4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Block;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	TArray<TObjectPtr<UTexture2D>> DigitTextures;

	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	TObjectPtr<UTexture2D> BlockTexture;

private:
	TArray<UImage*> GetDigitImages() const;
};
