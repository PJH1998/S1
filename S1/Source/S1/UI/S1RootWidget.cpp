// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1RootWidget.h"
#include "System/S1UIManager.h"
#include "UI/Cursor/S1Cursor.h"
#include "UI/Fade/S1Fade.h"
#include "UI/Lobby/S1HUD_Lobby.h"
#include "Framework/Application/SlateApplication.h"

US1RootWidget::US1RootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<US1HUD_Lobby> FindHUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Lobby/WBP_HUD_Lobby.WBP_HUD_Lobby_C'"));
	if (FindHUD.Succeeded())
	{
		HUDClass = FindHUD.Class;
	}
	ConstructorHelpers::FClassFinder<US1Cursor> FindCursor(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Cursor/WBP_Cursor.WBP_Cursor_C'"));
	if (FindCursor.Succeeded())
	{
		CursorClass = FindCursor.Class;
	}
	ConstructorHelpers::FClassFinder<US1Fade> FindFade(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Fade/WBP_Fade.WBP_Fade_C'"));
	if (FindFade.Succeeded())
	{
		FadeClass = FindFade.Class;
	}

	PanelSlots.SetNum(static_cast<int32>(UI_TYPE::END));
}

void US1RootWidget::FadeIn(float InDuration)
{
	if (FadeWidget == nullptr)
	{
		return;
	}

	FadeWidget->FadeIn(InDuration);
}

void US1RootWidget::FadeOut(float InDuration)
{
	if (FadeWidget == nullptr)
	{
		return;
	}

	FadeWidget->FadeOut(InDuration);
}

void US1RootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->Register_RootUI(this);
	}

	Register_Panel<US1HUD_Lobby>(UI_TYPE::HUD, HUDClass, CanvasPanel_HUD, false);
	if (PanelSlots[static_cast<int32>(UI_TYPE::HUD)] != nullptr)
	{
		PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetOffsets(FMargin(0.0f));
		PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetAlignment(FVector2D::ZeroVector);
	}

	Register_Panel<US1Cursor>(UI_TYPE::CURSOR, CursorClass, CanvasPanel_Cursor);
	CanvasPanel_Cursor->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)] != nullptr)
	{
		PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)]->SetAlignment(FVector2D(0.5f, 0.5f));
	}

	FadeWidget = Register_Panel<US1Fade>(UI_TYPE::FADE, FadeClass, CanvasPanel_Fade);
	CanvasPanel_Fade->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (PanelSlots[static_cast<int32>(UI_TYPE::FADE)] != nullptr)
	{
		PanelSlots[static_cast<int32>(UI_TYPE::FADE)]->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		PanelSlots[static_cast<int32>(UI_TYPE::FADE)]->SetOffsets(FMargin(0.0f));
		PanelSlots[static_cast<int32>(UI_TYPE::FADE)]->SetAlignment(FVector2D::ZeroVector);
	}

	// Fade Delegate
	FadeWidget->OnFadeFinished.AddDynamic(this, &ThisClass::DeliveFadeFinished);
}

void US1RootWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMousePosition(MyGeometry);
}

void US1RootWidget::NativeDestruct()
{
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->UnRegister_RootUI();
	}
}

void US1RootWidget::UpdateMousePosition(const FGeometry& MyGeometry)
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		return;
	}

	const FVector2D MouseAbsolutePos = FSlateApplication::Get().GetCursorPos();

	FVector2D MouseLocalPos = CanvasPanel_Cursor->GetCachedGeometry().AbsoluteToLocal(MouseAbsolutePos);
	PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)]->SetPosition(MouseLocalPos);
}

void US1RootWidget::DeliveFadeFinished()
{
	DeliverFinished.Broadcast();
}
