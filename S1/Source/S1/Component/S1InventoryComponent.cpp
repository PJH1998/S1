// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1InventoryComponent.h"
#include "S1Define.h"

US1InventoryComponent::US1InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	for (int32 Index = 0; Index < ItemStacks.Num(); ++Index)
	{
		FS1InventoryItemStack& ItemStack = ItemStacks[Index];
		if (ItemStack.ItemTag != ItemTag)
		{
			continue;
		}

		if (ItemStack.Count < Count)
		{
			return false;
		}

		ItemStack.Count -= Count;
		if (ItemStack.Count <= 0)
		{
			ItemStacks.RemoveAt(Index);
		}

		OnInventoryChanged.Broadcast();
		return true;
	}

	return false;
}

int32 US1InventoryComponent::GetItemCount(FGameplayTag ItemTag) const
{
	for (const FS1InventoryItemStack& ItemStack : ItemStacks)
	{
		if (ItemStack.ItemTag == ItemTag)
		{
			return ItemStack.Count;
		}
	}

	return 0;
}
