// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Logo/S1Logo.h"
#include "Components/TextBlock.h"

US1Logo::US1Logo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Logo::NativeConstruct()
{
	Super::NativeConstruct();
}

void US1Logo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Text_Logo->SetOpacity(Opacity);

	Opacity += OpacitySpeed * InDeltaTime * OpacityReverse;
	if (Opacity > 1.f) OpacityReverse = -1;
	else if (Opacity < 0.f) OpacityReverse = 1;

	Opacity = FMath::Clamp(Opacity, 0.f, 1.f);
}
