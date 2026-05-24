// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/AssetManager.h"
#include "S1LogChannels.h"
#include "GameplayTagContainer.h"
#include "Data/S1AssetData.h"
#include "S1AssetManager.generated.h"

DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&, UObject*);
DECLARE_DELEGATE(FAsyncLabelLoadCompletedDelegate);

UCLASS()
class S1_API US1AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	US1AssetManager();

	static US1AssetManager& Get();

	template<typename AssetType>
	static AssetType*	GetAssetByTag(const FGameplayTag& AssetTag);

public:
	static void			Initialize();

	static void			LoadAsyncByPath(const FSoftObjectPath& AssetPath, const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	static void			LoadAsyncByTag(const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	static void			LoadAsyncByLabel(const FGameplayTag& AssetTag, FAsyncLabelLoadCompletedDelegate CompletedDelegate = FAsyncLabelLoadCompletedDelegate());

	static void			ReleaseByPath(const FSoftObjectPath& AssetPath);
	static void			ReleaseByTag(const FGameplayTag& AssetTag);
	static void			ReleaseByLabel(const FGameplayTag& Label);
	static void			ReleaseAll();

	// Loading Progress - After the Call LoadAsyncByLabel
	static float		GetLoadingProgress();
	static TWeakPtr<FStreamableHandle> GetActiveLoadHandle();

private:
	void				LoadAssetsToLabel(const FGameplayTag& Label, FAsyncLabelLoadCompletedDelegate CompletedDelegate = FAsyncLabelLoadCompletedDelegate());

	void				AddLoadedAsset(const FName& AssetName, const FGameplayTag& AssetTag, const UObject* Asset);

private:

	UPROPERTY()
	TObjectPtr<US1AssetData> LoadedAssetData;

	UPROPERTY()
	TMap<FName, FGameplayTag> NameToTag;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<const UObject>> TagToLoadedAsset;

	TSharedPtr<FStreamableHandle> ActiveLoadHandle;
};

template<typename AssetType>
AssetType* US1AssetManager::GetAssetByTag(const FGameplayTag& AssetTag)
{
	US1AssetData* AssetData = Get().LoadedAssetData;
	if (!AssetData)
	{
		return nullptr;
	}

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath AssetPath = AssetData->GetAssetPathByTag(AssetTag);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		if (nullptr == LoadedAsset)
		{
			LOG_WARNING(TEXT("Attempted sync loading because asset hadn't loaded yet [%s]."), *AssetTag.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}
	return LoadedAsset;
}


#pragma region PDA_TAG VERSION
/*


UPROPERTY()
TMap<FGameplayTag, TObjectPtr<US1AssetData>> LoadedAssetData;


US1AssetData*		GetLoadadAssetByTag(const FGameplayTag& AssetTag);

template<typename AssetType>
static AssetType* GetAssetByTag(const FGameplayTag& DataTag, const FGameplayTag& AssetTag);

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
*/
#pragma endregion