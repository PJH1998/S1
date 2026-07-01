// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1RootWidget.h"
#include "S1RootWidget_Gameplay.generated.h"

class US1Inventory_ItemInfo;
class US1Menu_Inventory;

/**
 * 
 */
UCLASS()
class S1_API US1RootWidget_Gameplay : public US1RootWidget
{
	GENERATED_BODY()

public:
	US1RootWidget_Gameplay(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void ShowMenu(const FGameplayTag& UITag) override;
	virtual void HideAllMenus() override;
	virtual bool IsInventoryMenuOpen() const override;
	virtual US1Inventory_ItemInfo* GetItemInfoWidget() const override { return ItemInfoWidget; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Menu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Popup;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Menu_Inventory> Menu_Inventory;

private:
	TObjectPtr<US1Inventory_ItemInfo> ItemInfoWidget;
};