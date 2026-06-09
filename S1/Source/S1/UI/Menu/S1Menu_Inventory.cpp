// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Menu_Inventory.h"

#include "Framework/Application/SlateApplication.h"
#include "System/S1UIManager.h"
#include "UI/Menu/S1Inventory_ItemInfo.h"
#include "UI/Menu/S1Menu_Inventory_Bag.h"
#include "UI/Menu/S1Menu_Inventory_Equip.h"
#include "UI/S1RootWidget.h"

void US1Menu_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	if (Inventory_Equip)
	{
		Inventory_Equip->InitializeMenu(this);
	}

	if (Inventory_Bag)
	{
		Inventory_Bag->InitializeMenu(this);
	}

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		if (US1RootWidget* RootWidget = UIManager->GetRootWidget())
		{
			ItemInfoWidget = RootWidget->GetItemInfoWidget();
		}
	}
}

void US1Menu_Inventory::HandleSlotHoverStarted(FGameplayTag ItemTag, const FVector2D& ScreenPosition)
{
	if (ItemInfoWidget == nullptr)
	{
		return;
	}

	ItemInfoWidget->SetItemInfo(ItemTag);
	ItemInfoWidget->ShowAt(ScreenPosition);
}

void US1Menu_Inventory::HandleSlotHoverEnded()
{
	if (ItemInfoWidget == nullptr)
	{
		return;
	}

	ItemInfoWidget->HideInfo();
}
