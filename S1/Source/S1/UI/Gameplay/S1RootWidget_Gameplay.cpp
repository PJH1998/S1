// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/S1RootWidget_Gameplay.h"
#include "UI/Gameplay/S1Announce.h"
#include "UI/Menu/S1Inventory_ItemInfo.h"
#include "UI/Menu/S1Menu_Inventory.h"

#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "System/S1SoundManager.h"
#include "Data/S1UIData.h"

US1RootWidget_Gameplay::US1RootWidget_Gameplay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1RootWidget_Gameplay::NativeConstruct()
{
	Super::NativeConstruct();

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData))
	{
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

void US1RootWidget_Gameplay::ShowMenu(const FGameplayTag& UITag)
{
	if (UITag == S1UITags::UI_Menu_Inventory && Menu_Inventory)
	{
		const ESlateVisibility NewVisibility = Menu_Inventory->IsVisible() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
		Menu_Inventory->SetVisibility(NewVisibility);

		if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
		{
			const bool bOpened = (NewVisibility == ESlateVisibility::Visible);
			SoundManager->PlaySoundByTag(bOpened ? S1SoundTags::Sound_UI_Menu_Open : S1SoundTags::Sound_UI_Menu_Close);
		}

		if (NewVisibility == ESlateVisibility::Visible)
		{
			Menu_Inventory->HandleShown();
		}
	}
}

void US1RootWidget_Gameplay::HideAllMenus()
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

bool US1RootWidget_Gameplay::IsInventoryMenuOpen() const
{
	return Menu_Inventory && Menu_Inventory->IsVisible();
}

void US1RootWidget_Gameplay::ShowAnnounce(const FGameplayTag& AnnounceTag)
{
	if (Announce)
	{
		Announce->ShowAnnounce(AnnounceTag);
	}
}