// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/S1GameModeBase.h"
#include "Player/S1PlayerState.h"
#include "Data/S1CharacterSelectData.h"
#include "System/S1AssetManager.h"
#include "Character/Player/S1Player.h"
#include "Tags/Asset/S1GameplayTags_Asset.h"

AS1GameModeBase::AS1GameModeBase()
{
	bUseSeamlessTravel = true;
}

void AS1GameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

UClass* AS1GameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	FGameplayTag SelectedTag;

	if (AS1PlayerState* PS = InController->GetPlayerState<AS1PlayerState>())
	{
		SelectedTag = PS->GetSelectedCharacterTag();
	}

	if (SelectedTag.IsValid())
	{
		if (US1CharacterSelectData* SelectData = US1AssetManager::GetAssetByTag<US1CharacterSelectData>(S1AssetTags::Asset_CharacterData))
		{
			if (const FS1SelectCharacterEntry* Entry = SelectData->FindEntryByTag(SelectedTag))
			{
				if (nullptr != Entry->CharacterClass)
				{
					return Entry->CharacterClass.Get();
				}
			}
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
