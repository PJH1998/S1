// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1Menu_Inventory.generated.h"

class US1Inventory_ItemInfo;
class US1Menu_Inventory_Bag;
class US1Menu_Inventory_Equip;

UCLASS()
class S1_API US1Menu_Inventory : public US1BaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void HandleSlotHoverStarted(FGameplayTag ItemTag, const FVector2D& ScreenPosition);
	void HandleSlotHoverEnded();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Menu_Inventory_Equip> Inventory_Equip;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Menu_Inventory_Bag> Inventory_Bag;

	UPROPERTY(Transient)
	TObjectPtr<US1Inventory_ItemInfo> ItemInfoWidget;
};
