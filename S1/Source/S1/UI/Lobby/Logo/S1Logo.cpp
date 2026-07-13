// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Logo/S1Logo.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "System/S1UIManager.h"

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

FReply US1Logo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("Logo : BTN Down"));

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnLogoClickBeginDelegate.Broadcast();

		if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
		{
			UIManager->FadeOut(3.f, [this]()
				{
					//UGameplayStatics::OpenLevel(UIManager, FName(TEXT("Devmap")));
					Change_Customize();
				});
		}
	}

	return FReply::Handled();
}

void US1Logo::Change_Customize()
{
	OnClickedLogoDelegate.Broadcast();
}
