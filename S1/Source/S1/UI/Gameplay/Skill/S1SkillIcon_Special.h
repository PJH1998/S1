// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "UI/S1BaseWidget.h"
#include "S1SkillIcon_Special.generated.h"

class UImage;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UWidgetAnimation;
class US1PlayerSet;

/**
 *
 */
UCLASS()
class S1_API US1SkillIcon_Special : public US1BaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void InitializeGaugeMaterial();
	void InitializeIconTexture();
	void UpdateGaugeVisual();

	static FGameplayTag AppendGenderSuffix(const FGameplayTag& BaseTag, EPlayerGender Gender);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_BG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Gauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Screen;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Effect;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Effect_Flash;

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon|Gauge")
	TObjectPtr<UMaterialInstance> GaugeMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> GaugeMID;

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon|Gauge")
	FName FillRatioParameterName = TEXT("FillRatio");

	UPROPERTY(EditDefaultsOnly, Category = "SkillIcon|Gauge")
	FName LerpRatioParameterName = TEXT("LerpRatio");

private:
	UPROPERTY(Transient)
	TObjectPtr<US1PlayerSet> PlayerSet;

	float CurrentGaugeValue = {};
	float MaxGaugeValue = { 100.f };
	float LerpGaugeValue = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillIcon|Gauge", meta = (AllowPrivateAccess = "true"))
	float LerpSpeed = { 0.5f };

	bool bIconTextureInitialized = false;
	bool bWasGaugeFull = false;
};
