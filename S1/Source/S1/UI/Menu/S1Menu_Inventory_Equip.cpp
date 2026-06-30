// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Menu_Inventory_Equip.h"

#include "Component/S1EquipComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Player/S1PlayerState.h"
#include "UI/Menu/S1Inventory_Slot.h"
#include "UI/Menu/S1Menu_Inventory.h"

#include "Tags/S1GameplayTags.h"

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

	EnsureEquipBound();

	InitializeEquipSlots();
	RefreshSlots();
}

void US1Menu_Inventory_Equip::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 데디 클라에서 PlayerState가 늦게 복제되면 NativeConstruct 시점엔 바인딩이 안 된다.
	// 바인딩될 때까지만 재시도하고, 성사되면 한 번 갱신한다. (S1HPBar의 늦은 PlayerState 재취득 패턴)
	if (EquipComponent == nullptr)
	{
		EnsureEquipBound();
		if (EquipComponent != nullptr)
		{
			RefreshSlots();
		}
	}
}

void US1Menu_Inventory_Equip::HandleShown()
{
	RefreshSlots();
}

void US1Menu_Inventory_Equip::EnsureEquipBound()
{
	if (EquipComponent != nullptr)
	{
		return;
	}

	AS1PlayerState* PlayerState = GetOwnerPlayerState();
	if (PlayerState == nullptr)
	{
		return;
	}

	EquipComponent = PlayerState->GetEquipComponent();
	if (EquipComponent)
	{
		EquipComponent->OnEquipmentChanged.AddDynamic(this, &ThisClass::OnEquipmentChanged);
	}
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
			EquipComponent->RequestUnequipItem(EquipSlotEntry.Key);
			break;
		}
	}
}

void US1Menu_Inventory_Equip::RefreshSlots()
{
	EnsureEquipBound();

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
