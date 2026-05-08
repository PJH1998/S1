// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Fade.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFadeFinishedDelegate);

/**
 * 
 */
UCLASS()
class S1_API US1Fade : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1Fade(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeIn(float Duration = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeOut(float Duration = 1.f);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	enum class FADE_STATE
	{
		None,
		FadeIn,
		FadeOut
	};

private:
	void StartFade(FADE_STATE InFadeState, float InStartOpacity, float InTargetOpacity, float InDuration);
	void FinishFade();

public:
	FFadeFinishedDelegate		OnFadeFinished;

private:
	float CurrentOpacity = { 0.f };
	float StartOpacity = { 0.f };
	float TargetOpacity = { 0.f };
	float FadeElapsed = { 0.f };
	float FadeDuration = { 1.f };
	FADE_STATE FadeState = { FADE_STATE::None };
};
