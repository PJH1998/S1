// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1RootWidget.h"
#include "System/S1UIManager.h"
#include "UI/Cursor/S1Cursor.h"
#include "UI/Fade/S1Fade.h"
#include "UI/Lobby/S1HUD_Lobby.h"
#include "Framework/Application/SlateApplication.h"

#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1UIData.h"

US1RootWidget::US1RootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PanelSlots.SetNum(static_cast<int32>(UI_TYPE::END));
}

void US1RootWidget::SetUp_HUD(const FGameplayTag& UITag)
{
	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1DataTags::Data_Input, S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> HUDClass = UIData->FindUserWidgetClassByTag(UITag))
		{
			Register_Panel<US1BaseWidget>(UI_TYPE::HUD, HUDClass, CanvasPanel_HUD, false);
			if (PanelSlots[static_cast<int32>(UI_TYPE::HUD)] != nullptr)
			{
				PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
				PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetOffsets(FMargin(0.0f));
				PanelSlots[static_cast<int32>(UI_TYPE::HUD)]->SetAlignment(FVector2D::ZeroVector);
			}
		}
	}
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

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1DataTags::Data_Input, S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> CursorClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Cursor))
		{
			Register_Panel<US1BaseWidget>(UI_TYPE::CURSOR, CursorClass, CanvasPanel_Cursor);
			CanvasPanel_Cursor->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)] != nullptr)
			{
				PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)]->SetAlignment(FVector2D(0.5f, 0.5f));
			}
		}

		if (TSubclassOf<US1BaseWidget> FadeClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Fade))
		{
			FadeWidget = Cast<US1Fade>(Register_Panel<US1BaseWidget>(UI_TYPE::FADE, FadeClass, CanvasPanel_Fade));
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
	}
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
