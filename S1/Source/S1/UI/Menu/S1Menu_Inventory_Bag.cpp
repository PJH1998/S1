// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Menu_Inventory_Bag.h"

#include "Component/S1EquipComponent.h"
#include "Component/S1InventoryComponent.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Player/S1PlayerState.h"
#include "S1DataTableTypes.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1ItemManager.h"
#include "UI/Menu/S1Inventory_Slot.h"
#include "UI/Menu/S1Menu_Inventory.h"

void US1Menu_Inventory_Bag::InitializeMenu(US1Menu_Inventory* InOwnerMenu)
{
	OwnerMenu = InOwnerMenu;
}

void US1Menu_Inventory_Bag::NativeConstruct()
{
	Super::NativeConstruct();

	if (Tab_Equip)
	{
		Tab_Equip->OnClicked.AddDynamic(this, &ThisClass::OnTabEquipClicked);
	}

	if (Tab_Other)
	{
		Tab_Other->OnClicked.AddDynamic(this, &ThisClass::OnTabOtherClicked);
	}

	EnsureInventoryBound();

	EnsureSlotsCreated();
	RefreshSlots();
}

void US1Menu_Inventory_Bag::HandleShown()
{
	RefreshSlots();
}

void US1Menu_Inventory_Bag::EnsureInventoryBound()
{
	if (InventoryComponent != nullptr)
	{
		return;
	}

	AS1PlayerState* PlayerState = GetOwnerPlayerState();
	if (PlayerState == nullptr)
	{
		return;
	}

	InventoryComponent = PlayerState->GetInventoryComponent();
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &ThisClass::OnInventoryChanged);
	}
}

void US1Menu_Inventory_Bag::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &ThisClass::OnInventoryChanged);
	}

	Super::NativeDestruct();
}

void US1Menu_Inventory_Bag::OnTabEquipClicked()
{
	ActiveTab = ES1InventoryBagTab::Equip;
	RefreshSlots();
}

void US1Menu_Inventory_Bag::OnTabOtherClicked()
{
	ActiveTab = ES1InventoryBagTab::Other;
	RefreshSlots();
}

void US1Menu_Inventory_Bag::OnInventoryChanged()
{
	RefreshSlots();
}

void US1Menu_Inventory_Bag::OnSlotHoverStarted(FGameplayTag ItemTag)
{
	if (OwnerMenu == nullptr)
	{
		return;
	}

	const FVector2D ScreenPosition = FSlateApplication::Get().GetCursorPos();
	OwnerMenu->HandleSlotHoverStarted(ItemTag, ScreenPosition);
}

void US1Menu_Inventory_Bag::OnSlotHoverEnded()
{
	if (OwnerMenu)
	{
		OwnerMenu->HandleSlotHoverEnded();
	}
}

void US1Menu_Inventory_Bag::OnSlotRightClicked(FGameplayTag ItemTag, int32 SlotIndex)
{
	if (ActiveTab != ES1InventoryBagTab::Equip)
	{
		return;
	}

	AS1PlayerState* PlayerState = GetOwnerPlayerState();
	if (PlayerState == nullptr)
	{
		return;
	}

	const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	if (ItemManager == nullptr)
	{
		return;
	}

	const FS1ItemData* ItemData = ItemManager->FindItemData(ItemTag);
	if (ItemData == nullptr
		|| ItemData->ItemType != S1ItemTypeTags::Item_Type_Equip
		|| false == ItemData->EquipSlot.IsValid())
	{
		return;
	}

	if (US1EquipComponent* EquipComponent = PlayerState->GetEquipComponent())
	{
		EquipComponent->RequestEquipItem(ItemTag);
	}
}

void US1Menu_Inventory_Bag::RefreshSlots()
{
	EnsureInventoryBound();

	if (InventoryComponent == nullptr || GridPanel_Slots == nullptr)
	{
		return;
	}

	EnsureSlotsCreated();

	TArray<FGameplayTag> FilteredItemTags;
	TArray<int32> FilteredCounts;
	for (const FS1InventoryItemStack& ItemStack : InventoryComponent->GetItemStacks())
	{
		const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
		const FS1ItemData* ItemData = ItemManager ? ItemManager->FindItemData(ItemStack.ItemTag) : nullptr;
		if (ItemData == nullptr || false == PassesTabFilter(ItemData->ItemType))
		{
			continue;
		}

		FilteredItemTags.Add(ItemStack.ItemTag);
		FilteredCounts.Add(ItemStack.Count);
	}

	for (int32 Index = 0; Index < SlotWidgets.Num(); ++Index)
	{
		US1Inventory_Slot* SlotWidget = SlotWidgets[Index];
		if (SlotWidget == nullptr)
		{
			continue;
		}

		if (FilteredItemTags.IsValidIndex(Index))
		{
			SlotWidget->SetSlotData(FilteredItemTags[Index], FilteredCounts[Index], Index, false);
		}
		else
		{
			SlotWidget->ClearSlot();
		}
	}
}

void US1Menu_Inventory_Bag::EnsureSlotsCreated()
{
	if (GridPanel_Slots == nullptr || SlotWidgetClass == nullptr || SlotWidgets.Num() > 0)
	{
		return;
	}

	SlotWidgets.Reserve(MaxSlotCount);
	for (int32 Index = 0; Index < MaxSlotCount; ++Index)
	{
		US1Inventory_Slot* SlotWidget = CreateWidget<US1Inventory_Slot>(GetOwningPlayer(), SlotWidgetClass);
		if (SlotWidget == nullptr)
		{
			continue;
		}

		const int32 Row = Index / GridColumns;
		const int32 Column = Index % GridColumns;
		GridPanel_Slots->AddChildToUniformGrid(SlotWidget, Row, Column);

		SlotWidget->OnSlotHoverStarted.AddDynamic(this, &ThisClass::OnSlotHoverStarted);
		SlotWidget->OnSlotHoverEnded.AddDynamic(this, &ThisClass::OnSlotHoverEnded);
		SlotWidget->OnSlotRightClicked.AddDynamic(this, &ThisClass::OnSlotRightClicked);
		SlotWidgets.Add(SlotWidget);
	}
}

bool US1Menu_Inventory_Bag::PassesTabFilter(const FGameplayTag& ItemType) const
{
	if (ActiveTab == ES1InventoryBagTab::Equip)
	{
		return ItemType == S1ItemTypeTags::Item_Type_Equip;
	}

	return ItemType == S1ItemTypeTags::Item_Type_Inv;
}

AS1PlayerState* US1Menu_Inventory_Bag::GetOwnerPlayerState() const
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		return PlayerController->GetPlayerState<AS1PlayerState>();
	}

	return nullptr;
}
