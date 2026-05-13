// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1ProgressBar.generated.h"

class UImage;
class UMaterialInstance;
class UMaterialInstanceDynamic;

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
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetCurrentValue(float InValue);
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetMaxValue(float InValue);
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetValue(float InCurrentValue, float InMaxValue);

protected:
	virtual void NativeConstruct() override;

protected:
	virtual void	Bind_ShaderResource() {};
	float			GetFillRatio(float InValue) const;

private:
	void InitializeFillMaterial();


protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Fill;

	UPROPERTY(EditDefaultsOnly, Category = "ProgressBar")
	TObjectPtr<UMaterialInstance> GaugeMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> Fill_MID;

protected:

	float CurrentValue = {};
	float MaxValue = { 100.f };

	FName FillRatioParameterName = TEXT("FillRatio");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar", meta = (AllowPrivateAccess = "true"))
	float LerpSpeed = { 0.5f };
};
