// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Status/S1PlayerStatus.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1UIResource.h"
#include "Player/S1PlayerState.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

US1PlayerStatus::US1PlayerStatus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1PlayerStatus::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshCharacterInfo();
}

void US1PlayerStatus::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == bCharacterInfoInitialized)
	{
		RefreshCharacterInfo();
	}
}

void US1PlayerStatus::RefreshCharacterInfo()
{
	AS1PlayerState* PS = GetOwningPlayerState<AS1PlayerState>();
	const FGameplayTag SelectedCharacterTag = PS ? PS->GetSelectedCharacterTag() : FGameplayTag();
	if (false == SelectedCharacterTag.IsValid())
	{
		return;
	}

	const US1CharacterSelectData* CharacterSelectData = US1AssetManager::GetAssetByTag<US1CharacterSelectData>(S1AssetTags::Asset_CharacterData);
	const FS1SelectCharacterEntry* Entry = CharacterSelectData ? CharacterSelectData->FindEntryByTag(SelectedCharacterTag) : nullptr;
	if (Entry == nullptr)
	{
		return;
	}

	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	UTexture2D* PortraitTexture = UIResource ? UIResource->FindTextureByTag(SelectedCharacterTag) : nullptr;
	if (PortraitTexture == nullptr)
	{
		return;
	}

	if (Image_Char)
	{
		Image_Char->SetBrushFromTexture(PortraitTexture);
	}

	if (Text_Name)
	{
		Text_Name->SetText(Entry->DisplayName);
	}

	bCharacterInfoInitialized = true;
}
