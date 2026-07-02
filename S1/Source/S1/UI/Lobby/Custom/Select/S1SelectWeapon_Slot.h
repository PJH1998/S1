// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1SelectWeapon_Slot.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1SelectWeaponSlotClickedSignature);

/**
 *
 */
UCLASS()
class S1_API US1SelectWeapon_Slot : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void SetSelected(bool bInSelected);
	void SetWeaponTag(const FGameplayTag& InWeaponTag);
	FGameplayTag GetWeaponTag() const { return WeaponTag; }

	UPROPERTY(BlueprintAssignable)
	FS1SelectWeaponSlotClickedSignature OnSlotClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void UpdateFrameVisual();

	static FGameplayTag GetWeaponIconTag(const FGameplayTag& WeaponTag);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_BG;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(Transient)
	FGameplayTag WeaponTag;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> NormalBGTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> HoverBGTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> SelectedBGTexture;

	bool bHovered = false;
	bool bSelected = false;
};
