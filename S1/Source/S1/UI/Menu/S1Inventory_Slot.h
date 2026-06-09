// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1Inventory_Slot.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1InventorySlotHoverStartedSignature, FGameplayTag, ItemTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1InventorySlotHoverEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FS1InventorySlotRightClickedSignature, FGameplayTag, ItemTag, int32, SlotIndex);

UCLASS()
class S1_API US1Inventory_Slot : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void SetSlotData(FGameplayTag ItemTag, int32 Count, int32 InSlotIndex, bool bShowEquippedEffect);
	void ClearSlot();

	UPROPERTY(BlueprintAssignable)
	FS1InventorySlotHoverStartedSignature OnSlotHoverStarted;

	UPROPERTY(BlueprintAssignable)
	FS1InventorySlotHoverEndedSignature OnSlotHoverEnded;

	UPROPERTY(BlueprintAssignable)
	FS1InventorySlotRightClickedSignature OnSlotRightClicked;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void ReleaseVisuals();
	void ApplyTexture(UImage* Image, UTexture2D* Texture);
	static FGameplayTag GetRarityBGTag(const FGameplayTag& RarityTag);
	static FGameplayTag GetRarityFrameTag(const FGameplayTag& RarityTag);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_BG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Effect;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(Transient)
	FGameplayTag CachedItemTag;

	UPROPERTY(Transient)
	int32 CachedCount = 0;

	UPROPERTY(Transient)
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(Transient)
	bool CachedShowEquippedEffect = false;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedBG;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedFrame;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> CachedEffect;
};
