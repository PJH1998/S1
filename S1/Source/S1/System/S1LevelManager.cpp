// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1LevelManager.h"
#include "System/S1AssetManager.h"
#include "S1LogChannels.h"
#include "S1Define.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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

	
//	UGameplayStatics::OpenLevel(GetGameInstance(), S1Levels::LoadingMap);

	//Temp ( Loading 처리 전 테스트 용 )
	LoadResources();
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

	if (LoadAssetLabel.IsValid())
	{
		US1AssetManager::LoadAsyncByLabel(LoadAssetLabel, FAsyncLabelLoadCompletedDelegate::CreateLambda([this]()
			{
				bIsLoading = false;
				ServerTravelTo(LoadLevelName);
			}));
	}
	else
	{
		bIsLoading = false;
		ServerTravelTo(LoadLevelName);
	}
}

void US1LevelManager::ServerTravelTo(const FName& MapName)
{
	UGameInstance* GI = GetGameInstance();
	if (false == ::IsValid(GI))
	{
		return;
	}

	UWorld* World = GI->GetWorld();
	if (false == ::IsValid(World))
	{
		return;
	}

	// 서버 권위 전용 — 클라에서 호출되면 무시 (seamless travel은 서버가 모든 클라를 데려감)
	if (NM_Client == World->GetNetMode())
	{
		LOG_WARNING(TEXT("LevelManager : ServerTravel ignored on client"));
		return;
	}

	World->ServerTravel(MapName.ToString());
}
