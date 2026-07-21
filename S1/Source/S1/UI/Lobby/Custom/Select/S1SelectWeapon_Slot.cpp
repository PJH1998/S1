// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Custom/Select/S1SelectWeapon_Slot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/S1UIResource.h"
#include "Engine/Texture2D.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1ItemManager.h"
#include "System/S1SoundManager.h"
#include "Tags/S1GameplayTags.h"

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

	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	if (UIResource && Image_Icon)
	{
		Image_Icon->SetBrushFromTexture(UIResource->FindTextureByTag(GetWeaponIconTag(WeaponTag)));
	}

	const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	const FS1ItemData* ItemData = ItemManager ? ItemManager->FindItemData(WeaponTag) : nullptr;
	if (ItemData && Text_Name)
	{
		Text_Name->SetText(FText::FromName(ItemData->ItemName));
	}
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

		if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
		{
			SoundManager->PlaySoundByTag(S1SoundTags::Sound_UI_CharacterSelect);
		}

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

FGameplayTag US1SelectWeapon_Slot::GetWeaponIconTag(const FGameplayTag& WeaponTag)
{
	if (WeaponTag == S1ItemTags::Item_Weapon_DSWD01)
	{
		return S1UIResourceTags::UI_Icon_Weapon_DSWD;
	}
	if (WeaponTag == S1ItemTags::Item_Weapon_SWD00)
	{
		return S1UIResourceTags::UI_Icon_Weapon_SWD;
	}

	return S1UIResourceTags::UI_Icon_Weapon_RPR;
}

