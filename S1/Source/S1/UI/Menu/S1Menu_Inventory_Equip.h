// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1Menu_Inventory_Equip.generated.h"

class US1EquipComponent;
class US1Inventory_Slot;
class AS1PlayerState;
class US1Menu_Inventory;

UCLASS()
class S1_API US1Menu_Inventory_Equip : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void InitializeMenu(US1Menu_Inventory* InOwnerMenu);
	void RegisterEquipSlot(FGameplayTag SlotTag, US1Inventory_Slot* SlotWidget);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnEquipmentChanged();

	UFUNCTION()
	void OnSlotHoverStarted(FGameplayTag ItemTag);

	UFUNCTION()
	void OnSlotHoverEnded();

	UFUNCTION()
	void OnSlotRightClicked(FGameplayTag ItemTag, int32 SlotIndex);

	void RefreshSlots();
	void InitializeEquipSlots();
	AS1PlayerState* GetOwnerPlayerState() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Inventory_Slot> Slot_Weapon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Inventory_Slot> Slot_Costume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Inventory_Slot> Slot_Accessary;

private:
	UPROPERTY(Transient)
	TObjectPtr<US1Menu_Inventory> OwnerMenu;

	UPROPERTY(Transient)
	TObjectPtr<US1EquipComponent> EquipComponent;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<US1Inventory_Slot>> EquipSlotWidgets;
};
