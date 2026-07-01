// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Custom/Select/S1SelectWeapon_Slot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void US1SelectWeapon_Slot::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateFrameVisual();
}

void US1SelectWeapon_Slot::SetSelected(bool bInSelected)
{
	if (bSelected == bInSelected)
	{
		return;
	}

	bSelected = bInSelected;
	UpdateFrameVisual();
}

void US1SelectWeapon_Slot::SetWeaponTag(const FGameplayTag& InWeaponTag)
{
	WeaponTag = InWeaponTag;
}

void US1SelectWeapon_Slot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	bHovered = true;
	UpdateFrameVisual();
}

void US1SelectWeapon_Slot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	bHovered = false;
	UpdateFrameVisual();
}

FReply US1SelectWeapon_Slot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		SetSelected(true);
		OnSlotClicked.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void US1SelectWeapon_Slot::UpdateFrameVisual()
{
	if (Image_BG == nullptr)
	{
		return;
	}

	UTexture2D* TargetTexture = bSelected ? SelectedBGTexture : (bHovered ? HoverBGTexture : NormalBGTexture);
	if (TargetTexture)
	{
		Image_BG->SetBrushFromTexture(TargetTexture);
	}
}

