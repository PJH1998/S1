// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1RootWidget.h"
#include "System/S1UIManager.h"
#include "UI/Cursor/S1Cursor.h"
#include "UI/Fade/S1Fade.h"
#include "UI/Lobby/S1HUD_Lobby.h"
#include "UI/Menu/S1Inventory_ItemInfo.h"
#include "UI/Menu/S1Menu_Inventory.h"
#include "Framework/Application/SlateApplication.h"

#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1UIData.h"

US1RootWidget::US1RootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PanelSlots.SetNum(static_cast<int32>(UI_TYPE::END));
}

void US1RootWidget::ShowMenu(const FGameplayTag& UITag)
{
	if (UITag == S1UITags::UI_Menu_Inventory && Menu_Inventory)
	{
		const ESlateVisibility NewVisibility = Menu_Inventory->IsVisible() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
		Menu_Inventory->SetVisibility(NewVisibility);
	}
}

void US1RootWidget::HideAllMenus()
{
	if (Menu_Inventory)
	{
		Menu_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ItemInfoWidget)
	{
		ItemInfoWidget->HideInfo();
	}
}

bool US1RootWidget::IsInventoryMenuOpen() const
{
	return Menu_Inventory && Menu_Inventory->IsVisible();
}

void US1RootWidget::SetUp_HUD(const FGameplayTag& UITag)
{
	if (CanvasPanel_HUD->HasAnyChildren())
		CanvasPanel_HUD->ClearChildren();

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData))
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

		if (CanvasPanel_Popup)
		{
			if (TSubclassOf<US1BaseWidget> ItemInfoClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Menu_ItemInfo))
			{
				ItemInfoWidget = Cast<US1Inventory_ItemInfo>(Register_Panel<US1BaseWidget>(UI_TYPE::POPUP, ItemInfoClass, CanvasPanel_Popup));
				CanvasPanel_Popup->SetVisibility(ESlateVisibility::HitTestInvisible);
				if (PanelSlots[static_cast<int32>(UI_TYPE::POPUP)] != nullptr)
				{
					PanelSlots[static_cast<int32>(UI_TYPE::POPUP)]->SetAutoSize(true);
					PanelSlots[static_cast<int32>(UI_TYPE::POPUP)]->SetAlignment(FVector2D::ZeroVector);
				}

				if (ItemInfoWidget)
				{
					ItemInfoWidget->HideInfo();
				}
			}
		}
	}

	HideAllMenus();
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
