// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1RootWidget.h"
#include "UI/Cursor/S1Cursor.h"
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

	PanelSlots.SetNum(static_cast<int32>(UI_TYPE::END));
}

void US1RootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Register_Panel(UI_TYPE::HUD, HUDClass, false);
	Register_Panel(UI_TYPE::CURSOR, CursorClass);
}

void US1RootWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMousePosition(MyGeometry);
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