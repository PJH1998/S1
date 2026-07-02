// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1SkillIcon.generated.h"

class UImage;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UTexture2D;

/**
 * 
 */
UCLASS()
class S1_API US1SkillIcon : public US1BaseWidget
{
	GENERATED_BODY()
	
public:
	US1SkillIcon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

public:
	void SetIconTexture(UTexture2D* InTexture);
	void SetCooldown(float InRemainingTime, float InDuration);
	void SetUsable(bool bInUsable);

private:
	void InitializeCooldownMaterial();
	void UpdateCooldownVisual();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_BG;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Frame;

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon")
	TObjectPtr<UMaterialInstance> CooldownMaskMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> CooldownMaskMID;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon")
	FName CooldownRatioParameterName = TEXT("CooldownRatio");

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon")
	FName IconTextureParameterName = TEXT("IconTexture");

private:

	float CooldownRemainingTime = {};
	float CooldownDuration = {};
};
