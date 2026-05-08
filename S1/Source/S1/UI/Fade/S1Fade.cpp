// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Fade/S1Fade.h"

US1Fade::US1Fade(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Fade::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(CurrentOpacity);
	SetVisibility(ESlateVisibility::Collapsed);
}

void US1Fade::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (FadeState == FADE_STATE::None)
	{
		return;
	}

	FadeElapsed += InDeltaTime;
	const float Alpha = FMath::Clamp(FadeElapsed / FadeDuration, 0.f, 1.f);
	CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Alpha);
	SetRenderOpacity(CurrentOpacity);

	if (Alpha >= 1.f)
	{
		FinishFade();
	}
}

void US1Fade::FadeIn(float Duration)
{
	StartFade(FADE_STATE::FadeIn, CurrentOpacity, 0.f, Duration);
}

void US1Fade::FadeOut(float Duration)
{
	StartFade(FADE_STATE::FadeOut, CurrentOpacity, 1.f, Duration);
}

void US1Fade::StartFade(FADE_STATE InFadeState, float InStartOpacity, float InTargetOpacity, float InDuration)
{
	FadeState = InFadeState;
	StartOpacity = FMath::Clamp(InStartOpacity, 0.f, 1.f);
	TargetOpacity = FMath::Clamp(InTargetOpacity, 0.f, 1.f);
	FadeDuration = FMath::Max(InDuration, KINDA_SMALL_NUMBER);
	FadeElapsed = 0.f;
	CurrentOpacity = StartOpacity;

	SetRenderOpacity(CurrentOpacity);
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (InDuration <= 0.f)
	{
		CurrentOpacity = TargetOpacity;
		SetRenderOpacity(CurrentOpacity);
		FinishFade();
	}
}

void US1Fade::FinishFade()
{
	const FADE_STATE FinishedFadeState = FadeState;

	FadeState = FADE_STATE::None;
	CurrentOpacity = TargetOpacity;
	SetRenderOpacity(CurrentOpacity);

	OnFadeFinished.Broadcast();
	
	if (FadeState == FADE_STATE::None && FinishedFadeState == FADE_STATE::FadeIn)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}