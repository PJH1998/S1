// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1Menu_Inventory_Bag.generated.h"

class UButton;
class UUniformGridPanel;
class US1Inventory_Slot;
class US1InventoryComponent;
class AS1PlayerState;
class US1Menu_Inventory;

UENUM()
enum class ES1InventoryBagTab : uint8
{
	Equip,
	Other
};

UCLASS()
class S1_API US1Menu_Inventory_Bag : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void InitializeMenu(US1Menu_Inventory* InOwnerMenu);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnTabEquipClicked();

	UFUNCTION()
	void OnTabOtherClicked();

	UFUNCTION()
	void OnInventoryChanged();

	UFUNCTION()
	void OnSlotHoverStarted(FGameplayTag ItemTag);

	UFUNCTION()
	void OnSlotHoverEnded();

	UFUNCTION()
	void OnSlotRightClicked(FGameplayTag ItemTag, int32 SlotIndex);

	void RefreshSlots();
	void EnsureSlotsCreated();
	bool PassesTabFilter(const FGameplayTag& ItemType) const;
	AS1PlayerState* GetOwnerPlayerState() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Tab_Equip;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Tab_Other;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxSlotCount = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 GridColumns = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<US1Inventory_Slot> SlotWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<US1Menu_Inventory> OwnerMenu;

	UPROPERTY(Transient)
	TObjectPtr<US1InventoryComponent> InventoryComponent;

	UPROPERTY(Transient)
	ES1InventoryBagTab ActiveTab = ES1InventoryBagTab::Equip;

	UPROPERTY(Transient)
	TArray<TObjectPtr<US1Inventory_Slot>> SlotWidgets;
};
