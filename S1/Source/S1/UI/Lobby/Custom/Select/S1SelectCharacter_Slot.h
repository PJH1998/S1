// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1SelectCharacter_Slot.generated.h"

class UImage;
class UMaterialInterface;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1SelectCharacterSlotClickedSignature);

/**
 *
 */
UCLASS()
class S1_API US1SelectCharacter_Slot : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintAssignable)
	FS1SelectCharacterSlotClickedSignature OnSlotClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void UpdateFrameVisual();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Profile;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Frame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> ProfileMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> NormalFrameTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> HoverFrameTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Frame")
	TObjectPtr<UTexture2D> SelectedFrameTexture;

	bool bHovered = false;
	bool bSelected = false;
};
