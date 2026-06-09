// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1EquipComponent.h"

#include "Component/S1InventoryComponent.h"
#include "Player/S1PlayerState.h"
#include "S1DataTableTypes.h"
#include "S1GameplayTags.h"
#include "System/S1ItemManager.h"

US1EquipComponent::US1EquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool US1EquipComponent::EquipItem(FGameplayTag ItemTag)
{
	AS1PlayerState* PlayerState = Cast<AS1PlayerState>(GetOwner());
	if (PlayerState == nullptr || false == ItemTag.IsValid())
	{
		return false;
	}

	US1InventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent();
	if (InventoryComponent == nullptr || InventoryComponent->GetItemCount(ItemTag) < 1)
	{
		return false;
	}

	const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	const FS1ItemData* ItemData = ItemManager ? ItemManager->FindItemData(ItemTag) : nullptr;
	if (ItemData == nullptr
		|| ItemData->ItemType != S1ItemTypeTags::Item_Type_Equip
		|| false == ItemData->EquipSlot.IsValid())
	{
		return false;
	}

	const FGameplayTag SlotTag = ItemData->EquipSlot;
	if (GetEquippedItemTag(SlotTag).IsValid())
	{
		UnequipItem(SlotTag, true);
	}

	if (false == InventoryComponent->RemoveItem(ItemTag, 1))
	{
		return false;
	}

	for (FS1EquippedItem& EquippedItem : EquippedItems)
	{
		if (EquippedItem.SlotTag == SlotTag)
		{
			EquippedItem.ItemTag = ItemTag;
			OnItemEquipped.Broadcast(ItemTag);
			OnEquipmentChanged.Broadcast();
			return true;
		}
	}

	FS1EquippedItem NewEquippedItem;
	NewEquippedItem.SlotTag = SlotTag;
	NewEquippedItem.ItemTag = ItemTag;
	EquippedItems.Add(NewEquippedItem);

	// TODO: GAS additive stat GE apply
	OnItemEquipped.Broadcast(ItemTag);
	OnEquipmentChanged.Broadcast();
	return true;
}

bool US1EquipComponent::UnequipItem(FGameplayTag SlotTag, bool bFromEquipSwap)
{
	if (false == SlotTag.IsValid())
	{
		return false;
	}

	const FGameplayTag EquippedItemTag = GetEquippedItemTag(SlotTag);
	if (false == EquippedItemTag.IsValid())
	{
		return false;
	}

	AS1PlayerState* PlayerState = Cast<AS1PlayerState>(GetOwner());
	if (PlayerState == nullptr)
	{
		return false;
	}

	US1InventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent();
	if (InventoryComponent == nullptr || false == InventoryComponent->AddItem(EquippedItemTag, 1))
	{
		return false;
	}

	for (int32 Index = 0; Index < EquippedItems.Num(); ++Index)
	{
		if (EquippedItems[Index].SlotTag == SlotTag)
		{
			EquippedItems.RemoveAt(Index);
			break;
		}
	}

	// TODO: GAS additive stat GE remove
	if (false == bFromEquipSwap && SlotTag == S1EquipSlotTags::Equip_Type_Weapon)
	{
		OnItemEquipped.Broadcast(FGameplayTag());
	}

	OnEquipmentChanged.Broadcast();
	return true;
}

FGameplayTag US1EquipComponent::GetEquippedItemTag(FGameplayTag SlotTag) const
{
	for (const FS1EquippedItem& EquippedItem : EquippedItems)
	{
		if (EquippedItem.SlotTag == SlotTag)
		{
			return EquippedItem.ItemTag;
		}
	}

	return FGameplayTag();
}
