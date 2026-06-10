// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1EquipComponent.h"

#include "AbilitySystemComponent.h"
#include "Component/S1InventoryComponent.h"
#include "Data/S1GameplayEffectData.h"
#include "GameplayEffect.h"
#include "Player/S1PlayerState.h"
#include "S1DataTableTypes.h"
#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "System/S1ItemManager.h"

US1EquipComponent::US1EquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	EquipEffectHandles.SetNum(EquipSlotCount);
}

void US1EquipComponent::BeginPlay()
{
	Super::BeginPlay();

	if (US1GameplayEffectData* GameplayEffectData = US1AssetManager::GetAssetByTag<US1GameplayEffectData>(S1AssetTags::Asset_GameplayEffect))
	{
		ApplyEquipEffectClass = GameplayEffectData->FindEffectClassByTag(S1GameplayEffectTags::GameplayEffect_ApplyEquip);
	}
}

TOptional<ES1EquipSlot> US1EquipComponent::GetEquipSlotEnum(FGameplayTag SlotTag) const
{
	if (SlotTag == S1EquipSlotTags::Equip_Type_Weapon)
	{
		return ES1EquipSlot::Weapon;
	}
	if (SlotTag == S1EquipSlotTags::Equip_Type_Costume)
	{
		return ES1EquipSlot::Costume;
	}
	if (SlotTag == S1EquipSlotTags::Equip_Type_Accessary)
	{
		return ES1EquipSlot::Accessary;
	}

	return TOptional<ES1EquipSlot>();
}

FGameplayTag US1EquipComponent::GetEquipSlotTag(ES1EquipSlot Slot) const
{
	switch (Slot)
	{
	case ES1EquipSlot::Weapon:
		return S1EquipSlotTags::Equip_Type_Weapon;
	case ES1EquipSlot::Costume:
		return S1EquipSlotTags::Equip_Type_Costume;
	case ES1EquipSlot::Accessary:
		return S1EquipSlotTags::Equip_Type_Accessary;
	default:
		return FGameplayTag();
	}
}

bool US1EquipComponent::ApplyEquipGameplayEffect(ES1EquipSlot Slot, const FS1ItemData& ItemData)
{
	const int32 SlotIndex = static_cast<int32>(Slot);
	if (SlotIndex < 0 || SlotIndex >= EquipSlotCount)
	{
		return false;
	}

	AS1PlayerState* PlayerState = Cast<AS1PlayerState>(GetOwner());
	if (PlayerState == nullptr)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
	if (false == IsValid(AbilitySystemComponent))
	{
		return false;
	}

	if (ApplyEquipEffectClass == nullptr)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(ApplyEquipEffectClass, 1.f, Context);
	if (false == Spec.IsValid())
	{
		return false;
	}

	Spec.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Status_Health, ItemData.BaseHealth);
	Spec.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Status_Damage, ItemData.BaseDamage);

	const FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	EquipEffectHandles[SlotIndex] = ActiveHandle;
	return ActiveHandle.IsValid();
}

void US1EquipComponent::RemoveEquipGameplayEffect(ES1EquipSlot Slot)
{
	const int32 SlotIndex = static_cast<int32>(Slot);
	if (SlotIndex < 0 || SlotIndex >= EquipSlotCount)
	{
		return;
	}

	FActiveGameplayEffectHandle& ActiveHandle = EquipEffectHandles[SlotIndex];
	if (false == ActiveHandle.IsValid())
	{
		return;
	}

	AS1PlayerState* PlayerState = Cast<AS1PlayerState>(GetOwner());
	if (PlayerState == nullptr)
	{
		ActiveHandle.Invalidate();
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveHandle);
	}

	ActiveHandle.Invalidate();
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
	const TOptional<ES1EquipSlot> SlotEnum = GetEquipSlotEnum(SlotTag);
	if (false == SlotEnum.IsSet())
	{
		return false;
	}

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
			ApplyEquipGameplayEffect(SlotEnum.GetValue(), *ItemData);
			OnItemEquipped.Broadcast(ItemTag);
			OnEquipmentChanged.Broadcast();
			return true;
		}
	}

	FS1EquippedItem NewEquippedItem;
	NewEquippedItem.SlotTag = SlotTag;
	NewEquippedItem.ItemTag = ItemTag;
	EquippedItems.Add(NewEquippedItem);

	ApplyEquipGameplayEffect(SlotEnum.GetValue(), *ItemData);
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

	const TOptional<ES1EquipSlot> SlotEnum = GetEquipSlotEnum(SlotTag);
	if (false == SlotEnum.IsSet())
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

	RemoveEquipGameplayEffect(SlotEnum.GetValue());

	for (int32 Index = 0; Index < EquippedItems.Num(); ++Index)
	{
		if (EquippedItems[Index].SlotTag == SlotTag)
		{
			EquippedItems.RemoveAt(Index);
			break;
		}
	}

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
