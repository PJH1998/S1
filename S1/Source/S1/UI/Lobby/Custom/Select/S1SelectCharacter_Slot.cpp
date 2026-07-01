// Fill out your copyright notice in the Description page of Project Settings.


#include "S1SelectCharacter_Slot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void US1SelectCharacter_Slot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_Profile && ProfileMaterial)
	{
		Image_Profile->SetBrushFromMaterial(ProfileMaterial);
	}

	UpdateFrameVisual();
}

void US1SelectCharacter_Slot::SetSelected(bool bInSelected)
{
	if (bSelected == bInSelected)
	{
		return;
	}

	bSelected = bInSelected;
	UpdateFrameVisual();
}

void US1SelectCharacter_Slot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	bHovered = true;
	UpdateFrameVisual();
}

void US1SelectCharacter_Slot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	bHovered = false;
	UpdateFrameVisual();
}

FReply US1SelectCharacter_Slot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		SetSelected(true);
		OnSlotClicked.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void US1SelectCharacter_Slot::UpdateFrameVisual()
{
	if (Image_Frame == nullptr)
	{
		return;
	}

	UTexture2D* TargetTexture = bSelected ? SelectedFrameTexture : (bHovered ? HoverFrameTexture : NormalFrameTexture);
	if (TargetTexture)
	{
		Image_Frame->SetBrushFromTexture(TargetTexture);
	}
}

