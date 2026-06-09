// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Inventory_ItemInfo.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "S1DataTableTypes.h"
#include "System/S1ItemManager.h"

void US1Inventory_ItemInfo::SetItemInfo(FGameplayTag ItemTag)
{
	const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	const FS1ItemData* ItemData = ItemManager ? ItemManager->FindItemData(ItemTag) : nullptr;
	if (ItemData == nullptr)
	{
		HideInfo();
		return;
	}

	if (Text_Name)
	{
		Text_Name->SetText(FText::FromName(ItemData->ItemName));
	}

	if (Text_Stats)
	{
		const FText StatsText = FText::Format(
			NSLOCTEXT("S1Inventory", "ItemStats", "HP +{0}  ATK +{1}  DEF +{2}"),
			FText::AsNumber(FMath::RoundToInt(ItemData->BaseHealth)),
			FText::AsNumber(FMath::RoundToInt(ItemData->BaseDamage)),
			FText::AsNumber(FMath::RoundToInt(ItemData->BaseDefense)));
		Text_Stats->SetText(StatsText);
	}

	if (Text_Price)
	{
		Text_Price->SetText(FText::Format(NSLOCTEXT("S1Inventory", "ItemPrice", "Price: {0}"), FText::AsNumber(FMath::RoundToInt(ItemData->Price))));
	}
}

void US1Inventory_ItemInfo::ShowAt(const FVector2D& ScreenPosition)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasSlot->SetPosition(ScreenPosition + FVector2D(16.f, 16.f));
	}
}

void US1Inventory_ItemInfo::HideInfo()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
