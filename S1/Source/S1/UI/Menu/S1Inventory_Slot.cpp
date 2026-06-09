// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Menu/S1Inventory_Slot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/S1UIResource.h"
#include "Engine/Texture2D.h"
#include "S1DataTableTypes.h"
#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "System/S1ItemManager.h"

void US1Inventory_Slot::NativeConstruct()
{
	Super::NativeConstruct();
	CacheDefaultVisuals();
}

void US1Inventory_Slot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == bEffectAnimating || EquippedEffectFrameCount <= 0)
	{
		return;
	}

	EffectFrameTime += InDeltaTime;
	const float FrameDuration = 1.f / EquippedEffectFrameRate;
	while (EffectFrameTime >= FrameDuration)
	{
		EffectFrameTime -= FrameDuration;
		CurrentEffectFrame = (CurrentEffectFrame + 1) % EquippedEffectFrameCount;
		UpdateEffectSpriteFrame(CurrentEffectFrame);
	}
}

void US1Inventory_Slot::CacheDefaultVisuals()
{
	if (bDefaultVisualsCached)
	{
		return;
	}

	if (Image_BG)
	{
		DefaultBGBrush = Image_BG->GetBrush();
		DefaultBGVisibility = Image_BG->GetVisibility();
	}

	if (Image_Frame)
	{
		DefaultFrameBrush = Image_Frame->GetBrush();
		DefaultFrameVisibility = Image_Frame->GetVisibility();
	}

	bDefaultVisualsCached = true;
}

void US1Inventory_Slot::SetSlotData(FGameplayTag ItemTag, int32 Count, int32 InSlotIndex, bool bShowEquippedEffect)
{
	if (CachedItemTag == ItemTag && CachedCount == Count && CachedShowEquippedEffect == bShowEquippedEffect && SlotIndex == InSlotIndex)
	{
		return;
	}

	ReleaseVisuals();

	SlotIndex = InSlotIndex;
	if (false == ItemTag.IsValid())
	{
		return;
	}

	const US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	const FS1ItemData* ItemData = ItemManager ? ItemManager->FindItemData(ItemTag) : nullptr;
	if (ItemData == nullptr)
	{
		return;
	}

	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	if (UIResource == nullptr)
	{
		return;
	}

	CachedIcon = UIResource->FindTextureByTag(ItemTag);
	CachedBG = UIResource->FindTextureByTag(GetRarityBGTag(ItemData->Rarity));
	CachedFrame = UIResource->FindTextureByTag(GetRarityFrameTag(ItemData->Rarity));

	ApplyTexture(Image_BG, CachedBG);
	if (Image_BG)
	{
		Image_BG->SetVisibility(ESlateVisibility::Visible);
	}

	ApplyTexture(Image_Frame, CachedFrame);
	if (Image_Frame)
	{
		Image_Frame->SetVisibility(ESlateVisibility::Visible);
	}

	if (CachedIcon)
	{
		ApplyTexture(Image_Icon, CachedIcon);
		Image_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (bShowEquippedEffect && Image_Effect)
	{
		CachedEffect = UIResource->FindTextureByTag(S1UIResourceTags::UI_Icon_Effect_Equiped);
		if (CachedEffect)
		{
			StartEffectAnimation();
		}
	}

	if (Text_Count && Count > 1)
	{
		Text_Count->SetText(FText::AsNumber(Count));
		Text_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	CachedItemTag = ItemTag;
	CachedCount = Count;
	CachedShowEquippedEffect = bShowEquippedEffect;
}

void US1Inventory_Slot::ClearSlot()
{
	ReleaseVisuals();
}

void US1Inventory_Slot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CachedItemTag.IsValid())
	{
		OnSlotHoverStarted.Broadcast(CachedItemTag);
	}
}

void US1Inventory_Slot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (CachedItemTag.IsValid())
	{
		OnSlotHoverEnded.Broadcast();
	}
}

FReply US1Inventory_Slot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && CachedItemTag.IsValid())
	{
		OnSlotHoverEnded.Broadcast();
		OnSlotRightClicked.Broadcast(CachedItemTag, SlotIndex);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void US1Inventory_Slot::ReleaseVisuals()
{
	CachedItemTag = FGameplayTag();
	CachedCount = 0;
	SlotIndex = INDEX_NONE;
	CachedShowEquippedEffect = false;
	CachedIcon = nullptr;
	CachedBG = nullptr;
	CachedFrame = nullptr;
	CachedEffect = nullptr;

	if (Image_Icon)
	{
		Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Image_BG)
	{
		Image_BG->SetBrush(DefaultBGBrush);
		Image_BG->SetVisibility(DefaultBGVisibility);
	}

	if (Image_Frame)
	{
		Image_Frame->SetBrush(DefaultFrameBrush);
		Image_Frame->SetVisibility(DefaultFrameVisibility);
	}

	if (Image_Effect)
	{
		StopEffectAnimation();
		Image_Effect->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Text_Count)
	{
		Text_Count->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void US1Inventory_Slot::ApplyTexture(UImage* Image, UTexture2D* Texture)
{
	if (Image == nullptr || Texture == nullptr)
	{
		return;
	}

	Image->SetBrushFromTexture(Texture);
}

void US1Inventory_Slot::StartEffectAnimation()
{
	if (Image_Effect == nullptr || CachedEffect == nullptr || EquippedEffectFrameCount <= 0)
	{
		return;
	}

	CurrentEffectFrame = 0;
	EffectFrameTime = 0.f;
	bEffectAnimating = true;
	UpdateEffectSpriteFrame(0);
	Image_Effect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void US1Inventory_Slot::StopEffectAnimation()
{
	bEffectAnimating = false;
	CurrentEffectFrame = 0;
	EffectFrameTime = 0.f;
}

void US1Inventory_Slot::UpdateEffectSpriteFrame(int32 FrameIndex)
{
	if (Image_Effect == nullptr || CachedEffect == nullptr || EquippedEffectFrameCount <= 0)
	{
		return;
	}

	const int32 ClampedFrame = FMath::Clamp(FrameIndex, 0, EquippedEffectFrameCount - 1);
	const float InvFrameCount = 1.f / static_cast<float>(EquippedEffectFrameCount);
	const float VStart = ClampedFrame * InvFrameCount;
	const float VEnd = (ClampedFrame + 1) * InvFrameCount;

	const float FrameWidth = static_cast<float>(CachedEffect->GetSizeX());
	const float FrameHeight = static_cast<float>(CachedEffect->GetSizeY()) / static_cast<float>(EquippedEffectFrameCount);

	FSlateBrush Brush = Image_Effect->GetBrush();
	Brush.SetResourceObject(CachedEffect);
	Brush.ImageSize = FVector2D(FrameWidth, FrameHeight);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.SetUVRegion(FBox2f(FVector2f(0.f, VStart), FVector2f(1.f, VEnd)));
	Image_Effect->SetBrush(Brush);
}

FGameplayTag US1Inventory_Slot::GetRarityBGTag(const FGameplayTag& RarityTag)
{
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Rare)
	{
		return S1UIResourceTags::UI_Icon_BG_Rare;
	}
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Unique)
	{
		return S1UIResourceTags::UI_Icon_BG_Unique;
	}
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Legendary)
	{
		return S1UIResourceTags::UI_Icon_BG_Legendary;
	}

	return S1UIResourceTags::UI_Icon_BG_Common;
}

FGameplayTag US1Inventory_Slot::GetRarityFrameTag(const FGameplayTag& RarityTag)
{
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Rare)
	{
		return S1UIResourceTags::UI_Icon_Frame_Rare;
	}
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Unique)
	{
		return S1UIResourceTags::UI_Icon_Frame_Unique;
	}
	if (RarityTag == S1ItemRarityTags::Item_Rarity_Legendary)
	{
		return S1UIResourceTags::UI_Icon_Frame_Legendary;
	}

	return S1UIResourceTags::UI_Icon_Frame_Common;
}
