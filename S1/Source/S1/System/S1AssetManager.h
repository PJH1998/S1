// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/AssetManager.h"
#include "GameplayTagContainer.h"
#include "Data/S1AssetData.h"
#include "S1AssetManager.generated.h"

DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&, UObject*);
// FName = AssetName or Label
// UObject = LoadedAsset

UCLASS()
class S1_API US1AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	US1AssetManager();

	static US1AssetManager& Get();

	template<typename AssetType>
	static AssetType*	GetAssetByTag(const FGameplayTag& DataTag, const FGameplayTag& AssetTag);


public:
	static void			Initialize();

	static void			LoadAsyncByPath(const FSoftObjectPath& AssetPath, const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	static void			LoadAsyncByTag(const FGameplayTag& DataTag, const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());

	static void			LoadAsyncByLabel(const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());

	static void			ReleaseByPath(const FSoftObjectPath& AssetPath);
	static void			ReleaseByTag(const FGameplayTag& AssetTag);
	static void			ReleaseByLabel(const FGameplayTag& Label);
	static void			ReleaseAll();

private:
	US1AssetData*		GetLoadadAssetByTag(const FGameplayTag& AssetTag);
	void				LoadAssetsToLabel(const FGameplayTag& Label, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());

	void				AddLoadedAsset(const FName& AssetName, const FGameplayTag& AssetTag, const UObject* Asset);

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<US1AssetData>> LoadedAssetData;

	UPROPERTY()
	TMap<FName, FGameplayTag> NameToTag;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<const UObject>> TagToLoadedAsset;
};

template<typename AssetType>
AssetType* US1AssetManager::GetAssetByTag(const FGameplayTag& DataTag, const FGameplayTag& AssetTag)
{
	US1AssetData* AssetData = Get().GetLoadadAssetByTag(DataTag);
	if (!AssetData)
	{
		return nullptr;
	}

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath AssetPath = AssetData->GetAssetPathByTag(AssetTag);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		if (LoadedAsset == nullptr)
		{
			UE_LOG(LogWindows, Warning, TEXT("Attempted sync loading because asset hadn't loaded yet [%s]."), *AssetTag.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}
	return LoadedAsset;
}