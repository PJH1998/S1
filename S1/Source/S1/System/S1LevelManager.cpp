// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1LevelManager.h"
#include "System/S1AssetManager.h"
#include "S1LogChannels.h"
#include "S1Define.h"
#include "Kismet/GameplayStatics.h"

US1LevelManager* US1LevelManager::Get(const UObject* WorldContextObject)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
		return GI->GetSubsystem<US1LevelManager>();

	return nullptr;
}

void US1LevelManager::ChangeLevel(const FName& NextLevelName, const FGameplayTag& NextAssetLabel)
{
	if (bIsLoading)
	{
		LOG_WARNING(TEXT("LevelManager : Failed Change Level - Underway Resoure Loading"));
		return;
	}

	LoadLevelName = NextLevelName;
	LoadAssetLabel = NextAssetLabel;

	UGameplayStatics::OpenLevel(GetGameInstance(), S1Levels::LoadingMap);
}

void US1LevelManager::LoadResources()
{
	if (bIsLoading)
	{
		LOG_WARNING(TEXT("Failed LoadResources : Underway Resoure Loading"));
		return;
	}

	if (false == LoadLevelName.IsValid() || LoadLevelName.IsNone())
	{
		LOG_ERROR(TEXT("LevelManager : Failed Load Resource - None LoadLevelName"));
		return;
	}

	bIsLoading = true;

	US1AssetManager::LoadAsyncByLabel(LoadAssetLabel, FAsyncLabelLoadCompletedDelegate::CreateLambda([this]()
	{
		bIsLoading = false;
		UGameplayStatics::OpenLevel(GetGameInstance(), LoadLevelName);
	}));
}
