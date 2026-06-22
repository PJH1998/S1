// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1InventoryComponent.h"
#include "S1DataTableTypes.h"
#include "S1Define.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1ItemManager.h"

US1InventoryComponent::US1InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool US1InventoryComponent::IsStackableItem(FGameplayTag ItemTag) const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return true;
	}

	const US1ItemManager* ItemManager = World->GetSubsystem<US1ItemManager>();
	if (ItemManager == nullptr)
	{
		return true;
	}

	const FS1ItemData* ItemData = ItemManager->FindItemData(ItemTag);
	if (ItemData == nullptr)
	{
		return true;
	}

	return ItemData->ItemType != S1ItemTypeTags::Item_Type_Equip;
}

bool US1InventoryComponent::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	Gold += Amount;
	// Debug macro output. Remove after verifying gold pickup.
	D(FString::Printf(TEXT("Gold +%d | Total: %d"), Amount, Gold));
	OnGoldChanged.Broadcast(Gold);
	return true;
}

bool US1InventoryComponent::AddItem(FGameplayTag ItemTag, int32 Count)
{
	if (false == ItemTag.IsValid() || Count <= 0)
	{
		return false;
	}

	if (false == IsStackableItem(ItemTag))
	{
		for (int32 Index = 0; Index < Count; ++Index)
		{
			FS1InventoryItemStack NewItemStack;
			NewItemStack.ItemTag = ItemTag;
			NewItemStack.Count = 1;
			ItemStacks.Add(NewItemStack);
		}

		OnInventoryChanged.Broadcast();
		return true;
	}

	for (FS1InventoryItemStack& ItemStack : ItemStacks)
	{
		if (ItemStack.ItemTag == ItemTag)
		{
			ItemStack.Count += Count;
			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	FS1InventoryItemStack NewItemStack;
	NewItemStack.ItemTag = ItemTag;
	NewItemStack.Count = Count;
	ItemStacks.Add(NewItemStack);

	OnInventoryChanged.Broadcast();
	return true;
}

bool US1InventoryComponent::RemoveItem(FGameplayTag ItemTag, int32 Count)
{
	if (false == ItemTag.IsValid() || Count <= 0)
	{
		return false;
	}

	if (GetItemCount(ItemTag) < Count)
	{
		return false;
	}

	int32 RemainingToRemove = Count;
	for (int32 Index = ItemStacks.Num() - 1; Index >= 0 && RemainingToRemove > 0; --Index)
	{
		FS1InventoryItemStack& ItemStack = ItemStacks[Index];
		if (ItemStack.ItemTag != ItemTag)
		{
			continue;
		}

		const int32 RemoveFromStack = FMath::Min(ItemStack.Count, RemainingToRemove);
		ItemStack.Count -= RemoveFromStack;
		RemainingToRemove -= RemoveFromStack;

		if (ItemStack.Count <= 0)
		{
			ItemStacks.RemoveAt(Index);
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

int32 US1InventoryComponent::GetItemCount(FGameplayTag ItemTag) const
{
	int32 TotalCount = 0;
	for (const FS1InventoryItemStack& ItemStack : ItemStacks)
	{
		if (ItemStack.ItemTag == ItemTag)
		{
			TotalCount += ItemStack.Count;
		}
	}

	return TotalCount;
}
