// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Menu_Inventory_Equip.h"

#include "Component/S1EquipComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Player/S1PlayerState.h"
#include "UI/Menu/S1Inventory_Slot.h"
#include "UI/Menu/S1Menu_Inventory.h"

#include "S1GameplayTags.h"

void US1Menu_Inventory_Equip::InitializeMenu(US1Menu_Inventory* InOwnerMenu)
{
	OwnerMenu = InOwnerMenu;
}

void US1Menu_Inventory_Equip::RegisterEquipSlot(FGameplayTag SlotTag, US1Inventory_Slot* SlotWidget)
{
	if (false == SlotTag.IsValid() || SlotWidget == nullptr)
	{
		return;
	}

	EquipSlotWidgets.Add(SlotTag, SlotWidget);
	SlotWidget->OnSlotHoverStarted.AddDynamic(this, &ThisClass::OnSlotHoverStarted);
	SlotWidget->OnSlotHoverEnded.AddDynamic(this, &ThisClass::OnSlotHoverEnded);
	SlotWidget->OnSlotRightClicked.AddDynamic(this, &ThisClass::OnSlotRightClicked);
}

void US1Menu_Inventory_Equip::NativeConstruct()
{
	Super::NativeConstruct();

	if (AS1PlayerState* PlayerState = GetOwnerPlayerState())
	{
		EquipComponent = PlayerState->GetEquipComponent();
		if (EquipComponent)
		{
			EquipComponent->OnEquipmentChanged.AddDynamic(this, &ThisClass::OnEquipmentChanged);
		}
	}

	InitializeEquipSlots();
	RefreshSlots();
}

void US1Menu_Inventory_Equip::NativeDestruct()
{
	if (EquipComponent)
	{
		EquipComponent->OnEquipmentChanged.RemoveDynamic(this, &ThisClass::OnEquipmentChanged);
	}

	Super::NativeDestruct();
}

void US1Menu_Inventory_Equip::InitializeEquipSlots()
{
	RegisterEquipSlot(S1EquipSlotTags::Equip_Type_Weapon, Slot_Weapon);
	RegisterEquipSlot(S1EquipSlotTags::Equip_Type_Costume, Slot_Costume);
	RegisterEquipSlot(S1EquipSlotTags::Equip_Type_Accessary, Slot_Accessary);
}

void US1Menu_Inventory_Equip::OnEquipmentChanged()
{
	RefreshSlots();
}

void US1Menu_Inventory_Equip::OnSlotHoverStarted(FGameplayTag ItemTag)
{
	if (OwnerMenu == nullptr)
	{
		return;
	}

	const FVector2D ScreenPosition = FSlateApplication::Get().GetCursorPos();
	OwnerMenu->HandleSlotHoverStarted(ItemTag, ScreenPosition);
}

void US1Menu_Inventory_Equip::OnSlotHoverEnded()
{
	if (OwnerMenu)
	{
		OwnerMenu->HandleSlotHoverEnded();
	}
}

void US1Menu_Inventory_Equip::OnSlotRightClicked(FGameplayTag ItemTag, int32 SlotIndex)
{
	if (EquipComponent == nullptr)
	{
		return;
	}

	for (const TPair<FGameplayTag, TObjectPtr<US1Inventory_Slot>>& EquipSlotEntry : EquipSlotWidgets)
	{
		if (EquipComponent->GetEquippedItemTag(EquipSlotEntry.Key) == ItemTag)
		{
			EquipComponent->UnequipItem(EquipSlotEntry.Key);
			break;
		}
	}
}

void US1Menu_Inventory_Equip::RefreshSlots()
{
	if (EquipComponent == nullptr)
	{
		return;
	}

	for (const TPair<FGameplayTag, TObjectPtr<US1Inventory_Slot>>& EquipSlotEntry : EquipSlotWidgets)
	{
		US1Inventory_Slot* SlotWidget = EquipSlotEntry.Value;
		if (SlotWidget == nullptr)
		{
			continue;
		}

		const FGameplayTag EquippedItemTag = EquipComponent->GetEquippedItemTag(EquipSlotEntry.Key);
		if (EquippedItemTag.IsValid())
		{
			SlotWidget->SetSlotData(EquippedItemTag, 1, 0, true);
		}
		else
		{
			SlotWidget->ClearSlot();
		}
	}
}

AS1PlayerState* US1Menu_Inventory_Equip::GetOwnerPlayerState() const
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		return PlayerController->GetPlayerState<AS1PlayerState>();
	}

	return nullptr;
}
