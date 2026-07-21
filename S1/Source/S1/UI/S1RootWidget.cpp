// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1RootWidget.h"
#include "GameFramework/PlayerController.h"
#include "System/S1UIManager.h"
#include "UI/Cursor/S1Cursor.h"
#include "UI/Fade/S1Fade.h"
#include "Framework/Application/SlateApplication.h"

#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1UIData.h"

US1RootWidget::US1RootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PanelSlots.SetNum(static_cast<int32>(UI_TYPE::END));
	PanelWidgets.SetNum(static_cast<int32>(UI_TYPE::END));
}

US1BaseWidget* US1RootWidget::GetPanelWidget(UI_TYPE PanelType) const
{
	const int32 LocalType = static_cast<int32>(PanelType);
	if (false == PanelWidgets.IsValidIndex(LocalType))
	{
		return nullptr;
	}

	return PanelWidgets[LocalType];
}

UCanvasPanel* US1RootWidget::GetCanvasPanel(UI_TYPE PanelType) const
{
	switch (PanelType)
	{
	case UI_TYPE::HUD:    return CanvasPanel_HUD;
	case UI_TYPE::CURSOR: return CanvasPanel_Cursor;
	case UI_TYPE::FADE:   return CanvasPanel_Fade;
	default:              return nullptr;
	}
}

void US1RootWidget::SetUp_Panel(UI_TYPE PanelType, const FGameplayTag& UITag)
{
	UCanvasPanel* CanvasPanel = GetCanvasPanel(PanelType);
	if (CanvasPanel == nullptr)
	{
		return;
	}

	if (CanvasPanel->HasAnyChildren())
		CanvasPanel->ClearChildren();

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> WidgetClass = UIData->FindUserWidgetClassByTag(UITag))
		{
			Register_Panel<US1BaseWidget>(PanelType, WidgetClass, CanvasPanel, false);
			const int32 LocalType = static_cast<int32>(PanelType);
			if (PanelSlots[LocalType] != nullptr)
			{
				PanelSlots[LocalType]->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
				PanelSlots[LocalType]->SetOffsets(FMargin(0.0f));
				PanelSlots[LocalType]->SetAlignment(FVector2D::ZeroVector);
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

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> CursorClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Cursor))
		{
			Register_Panel<US1BaseWidget>(UI_TYPE::CURSOR, CursorClass, CanvasPanel_Cursor);
			CanvasPanel_Cursor->SetVisibility(ESlateVisibility::Collapsed);
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

void US1RootWidget::SetCursorVisible(bool bVisible)
{
	bCursorVisible = bVisible;

	if (CanvasPanel_Cursor)
	{
		CanvasPanel_Cursor->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void US1RootWidget::UpdateMousePosition(const FGeometry& MyGeometry)
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		return;
	}

	// 커서가 숨겨진 평소 상태에서는 화면 중앙에 고정, 보이는 동안에만 마우스를 따라간다.
	if (false == bCursorVisible)
	{
		PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)]->SetPosition(MyGeometry.GetLocalSize() * 0.5f);
		return;
	}

	const FVector2D MouseAbsolutePos = FSlateApplication::Get().GetCursorPos();

	//FVector2D MouseLocalPos = CanvasPanel_Cursor->GetCachedGeometry().AbsoluteToLocal(MouseAbsolutePos);
	FVector2D MouseLocalPos = MyGeometry.AbsoluteToLocal(MouseAbsolutePos);
	PanelSlots[static_cast<int32>(UI_TYPE::CURSOR)]->SetPosition(MouseLocalPos);
}

void US1RootWidget::DeliveFadeFinished()
{
	DeliverFinished.Broadcast();
}
