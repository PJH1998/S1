// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1EquipComponent.h"

US1EquipComponent::US1EquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool US1EquipComponent::EquipItem(FGameplayTag ItemTag)
{
	// TODO: ItemData 검증, 인벤 차감, GAS 스탯 적용
	return false;
}

bool US1EquipComponent::UnequipItem(FGameplayTag SlotTag)
{
	// TODO: GE 제거, 인벤 반환
	return false;
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
