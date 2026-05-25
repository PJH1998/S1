// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"

US1AssetManager::US1AssetManager()
    : Super()
{
}

US1AssetManager& US1AssetManager::Get()
{
    if (US1AssetManager* SingleTon = Cast<US1AssetManager>(GEngine->AssetManager))
    {
        return *SingleTon;
    }

    UE_LOG(LogWindows, Fatal, TEXT("Can't Find AssetManager"));

    return *(NewObject<US1AssetManager>());
}

void US1AssetManager::Initialize()
{
    Get().LoadAssetsToLabel(S1Labels::Label_Preload, FAsyncLabelLoadCompletedDelegate::CreateLambda([]()
    {
        // TODO Load Complete
        LOG(TEXT("Preload Assets Complete"));

    }));
}

void US1AssetManager::LoadAsyncByPath(const FSoftObjectPath& AssetPath, const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate)
{
    if (false == UAssetManager::IsInitialized())
    {
        LOG_ERROR(TEXT("AssetManager must be initialized"));
        return;
    }

    if (AssetPath.IsValid())
    {
        if (UObject* LoadedAsset = AssetPath.ResolveObject())
        {
            const FName& AssetName = AssetPath.GetAssetFName();
            Get().AddLoadedAsset(AssetName, AssetTag, LoadedAsset);
        }
        else
        {
            TArray<FSoftObjectPath> AssetPaths;
            AssetPaths.Add(AssetPath);

            TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);

            Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetName = AssetPath.GetAssetFName(), AssetTag, AssetPath, CompleteDelegate = MoveTemp(CompletedDelegate)]()
                {
                    UObject* LoadedAsset = AssetPath.ResolveObject();
                    Get().AddLoadedAsset(AssetName, AssetTag, LoadedAsset);
                    if (CompleteDelegate.IsBound())
                        CompleteDelegate.Execute(AssetName, LoadedAsset);
                }));
        }
    }
}

void US1AssetManager::LoadAsyncByTag(const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate)
{
    if (false == UAssetManager::IsInitialized())
    {
        LOG_ERROR(TEXT("AssetManager must be initialized"));
        return;
    }

    US1AssetData* AssetData = Get().LoadedAssetData;
    check(AssetData);

    const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByTag(AssetTag);
    LoadAsyncByPath(AssetPath, AssetTag, CompletedDelegate);
}

void US1AssetManager::LoadAsyncByLabel(const FGameplayTag& Label, FAsyncLabelLoadCompletedDelegate CompletedDelegate)
{
    if (false == UAssetManager::IsInitialized())
    {
        LOG_ERROR(TEXT("AssetManager must be initialized"));
        return;
    }

    // 모든 AssetData에서 해당 Label을 가진 Entry 수집
    TArray<FSoftObjectPath> PathsToLoad;
    TArray<FAssetEntry> EntriesToLoad;

    const US1AssetData* AssetData = Get().LoadedAssetData;
    check(AssetData);

    const FAssetSet* AssetSet = AssetData->FindAssetSetByLabel(Label);
    check(AssetSet);

    for (const FAssetEntry& Entry : AssetSet->AssetEntries)
    {
        if (Entry.AssetPath.IsValid())
        {
            PathsToLoad.Add(Entry.AssetPath);
            EntriesToLoad.Add(Entry);
        }
    }

    if (PathsToLoad.IsEmpty())
    {
        LOG_WARNING(TEXT("None Asset to Label : [%s]."), *Label.ToString());
        return;
    }

    US1AssetManager* AssetManager = &Get();

    AssetManager->ActiveLoadHandle = GetStreamableManager().RequestAsyncLoad(
        PathsToLoad,
        FStreamableDelegate::CreateLambda([AssetManager, EntriesToLoad, CompletedDelegate]()
        {
            for (const FAssetEntry& Entry : EntriesToLoad)
            {
                if (UObject* Loaded = Entry.AssetPath.ResolveObject())
                {
                    const FName& AssetName = Entry.AssetPath.GetAssetFName();
                    AssetManager->AddLoadedAsset(AssetName, Entry.AssetTag, Loaded);
                }
                else
                {
                    LOG_WARNING(TEXT("Failed to resolve asset [%s]"), *Entry.AssetPath.ToString());
                }
            }

            if (CompletedDelegate.IsBound())
            {
                CompletedDelegate.Execute();
            }

            AssetManager->ActiveLoadHandle.Reset();
        })
    );
}

float US1AssetManager::GetLoadingProgress()
{
    const TSharedPtr<FStreamableHandle>& Handle = Get().ActiveLoadHandle;
    if (Handle.IsValid())
        return Handle->GetProgress();

    LOG_WARNING(TEXT("None Loading Handle."));
    return 1.f;
}

TWeakPtr<FStreamableHandle> US1AssetManager::GetActiveLoadHandle()
{
    const TSharedPtr<FStreamableHandle>& Handle = Get().ActiveLoadHandle;
    if (Handle.IsValid())
        return TWeakPtr<FStreamableHandle>(Handle);

    LOG_WARNING(TEXT("GetActiveLoadHandle: None ActiveLoadHandle, Check After the LoadAsyncByLabel"));
    return TWeakPtr<FStreamableHandle>();
}

void US1AssetManager::ReleaseByPath(const FSoftObjectPath& AssetPath)
{
    FName AssetName = AssetPath.GetAssetFName();
    US1AssetManager& AssetManager = Get();

    FGameplayTag* FoundTag = AssetManager.NameToTag.Find(AssetName);
    if (!FoundTag)
    {
        LOG_WARNING(TEXT("ReleaseByPath: None Loaded Asset, Path - [%s]"), *AssetPath.ToString());
        return;
    }

    AssetManager.TagToLoadedAsset.Remove(*FoundTag);
    AssetManager.NameToTag.Remove(AssetName);
}

void US1AssetManager::ReleaseByTag(const FGameplayTag& AssetTag)
{
    US1AssetManager& AssetManager = Get();

    if (!AssetManager.TagToLoadedAsset.Contains(AssetTag))
    {
        LOG_WARNING(TEXT("ReleaseByTag: None Loaded Asset, Tag - [%s]"), *AssetTag.ToString());
        return;
    }

    AssetManager.TagToLoadedAsset.Remove(AssetTag);

    // NameToTag는 Value(Tag)로 찾아야 하므로 순회 삭제
    for (auto It = AssetManager.NameToTag.CreateIterator(); It; ++It)
    {
        if (It.Value() == AssetTag)
        {
            It.RemoveCurrent();
            break;  // Tag - Name 1:1 매핑이므로 찾으면 즉시 종료
        }
    }
}

void US1AssetManager::ReleaseByLabel(const FGameplayTag& Label)
{
    US1AssetManager& AssetManager = Get();
    US1AssetData* LoadedAssetData = AssetManager.LoadedAssetData;
    check(LoadedAssetData);

    // 제거할 Tag 목록 수집
    TArray<FGameplayTag> TagsToRemove;

    const FAssetSet* AssetSet = LoadedAssetData->FindAssetSetByLabel(Label);
    check(AssetSet);

    for (const FAssetEntry& AssetEntry : AssetSet->AssetEntries)
    {
        TagsToRemove.Add(AssetEntry.AssetTag);
    }
    // TagToLoadedAsset 제거
    for (const FGameplayTag& Tag : TagsToRemove)
    {
        AssetManager.TagToLoadedAsset.Remove(Tag);
    }

    // NameToTag에서 Value가 일치하는 항목 순회 삭제
    for (auto It = AssetManager.NameToTag.CreateIterator(); It; ++It)
    {
        if (TagsToRemove.Contains(It.Value()))
        {
            It.RemoveCurrent();
        }
    }
}

void US1AssetManager::ReleaseAll()
{
    US1AssetManager& AssetManager = Get();

    AssetManager.LoadedAssetData = nullptr;
    AssetManager.TagToLoadedAsset.Reset();
    AssetManager.NameToTag.Reset();
}

void US1AssetManager::LoadAssetsToLabel(const FGameplayTag& Label, FAsyncLabelLoadCompletedDelegate CompletedDelegate)
{
    if (LoadedAssetData)
    {
        LoadAsyncByLabel(Label, MoveTemp(CompletedDelegate));
    }
    else
    {
        US1AssetData* AssetData = nullptr;
        FPrimaryAssetType PrimaryAssetType(US1AssetData::StaticClass()->GetFName());
        TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);

        if (Handle.IsValid())
        {
            Handle->WaitUntilComplete(0.f, false);
            AssetData = Cast<US1AssetData>(Handle->GetLoadedAsset());
        }

        if (AssetData)
        {
            LoadedAssetData = AssetData;
            LoadAsyncByLabel(Label, MoveTemp(CompletedDelegate));
        }
        else
        {
            LOG_FATAL(TEXT("Failed to load AssetData asset type [%s]."), *PrimaryAssetType.ToString());
        }
    }
}

void US1AssetManager::AddLoadedAsset(const FName& AssetName, const FGameplayTag& AssetTag, const UObject* Asset)
{
    if (AssetTag.IsValid() && Asset)
    {
        //FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);

        if (false == NameToTag.Contains(AssetName))
        {
            NameToTag.Emplace(AssetName, AssetTag);
        }

        if (false == TagToLoadedAsset.Contains(AssetTag))
        {
            TagToLoadedAsset.Emplace(AssetTag, Asset);
        }
    }
}


#pragma region PDA_TAG VERSION
/*
void US1AssetManager::LoadAsyncByTag(const FGameplayTag& DataTag, const FGameplayTag& AssetTag, FAsyncLoadCompletedDelegate CompletedDelegate)
{
    if (false == UAssetManager::IsInitialized())
    {
        UE_LOG(LogWindows, Error, TEXT("AssetManager must be initialized"));
        return;
    }

    US1AssetData* AssetData = Get().GetLoadadAssetByTag(DataTag);
    check(AssetData);

    const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByTag(AssetTag);
    LoadAsyncByPath(AssetPath, AssetTag, CompletedDelegate);
}


void US1AssetManager::LoadAsyncByLabel(const FGameplayTag& Label, FAsyncLoadCompletedDelegate CompletedDelegate)
{
    if (false == UAssetManager::IsInitialized())
    {
        UE_LOG(LogWindows, Error, TEXT("AssetManager must be initialized"));
        return;
    }

    // 모든 AssetData에서 해당 Label을 가진 Entry 수집
    TArray<FSoftObjectPath> PathsToLoad;
    TArray<FAssetEntry> EntriesToLoad;

    for (const auto& Pair : Get().LoadedAssetData)
    {
        const US1AssetData* AssetData = Pair.Value.Get();
        if (!AssetData) continue;

        const FAssetSet* AssetSet = AssetData->FindAssetSetByLabel(Label);
        if (!AssetSet) continue;  // 해당 Label 없는 AssetData는 스킵

        for (const FAssetEntry& Entry : AssetSet->AssetEntries)
        {
            if (Entry.AssetPath.IsValid())
            {
                PathsToLoad.Add(Entry.AssetPath);
                EntriesToLoad.Add(Entry);
            }
        }
    }


if (PathsToLoad.IsEmpty())
{
    UE_LOG(LogWindows, Warning, TEXT("LoadAsyncByLabel: Label [%s] 에 해당하는 에셋 없음"), *Label.ToString());
    return;
}

US1AssetManager* AssetManager = &Get();

GetStreamableManager().RequestAsyncLoad(
    PathsToLoad,
    FStreamableDelegate::CreateLambda([AssetManager, EntriesToLoad, CompletedDelegate]()
        {
            for (const FAssetEntry& Entry : EntriesToLoad)
            {
                if (UObject* Loaded = Entry.AssetPath.ResolveObject())
                {
                    const FName& AssetName = Entry.AssetPath.GetAssetFName();
                    AssetManager->AddLoadedAsset(AssetName, Entry.AssetTag, Loaded);
                }
                else
                {
                    UE_LOG(LogWindows, Warning, TEXT("Failed to resolve asset [%s]"), *Entry.AssetPath.ToString());
                }
            }

            if (CompletedDelegate.IsBound())
            {
                CompletedDelegate.Execute(NAME_None, nullptr);
            }
        })
);
}

void US1AssetManager::ReleaseByLabel(const FGameplayTag& Label)
{
    US1AssetManager& AssetManager = Get();

    // 제거할 Tag 목록 수집
    TArray<FGameplayTag> TagsToRemove;
    for (const auto& LoadDataPair : AssetManager.LoadedAssetData)
    {
        const US1AssetData* AssetData = LoadDataPair.Value.Get();
        if (!AssetData) continue;

        const FAssetSet* AssetSet = AssetData->FindAssetSetByLabel(Label);
        if (!AssetSet) continue;

        for (const FAssetEntry& AssetEntry : AssetSet->AssetEntries)
        {
            TagsToRemove.Add(AssetEntry.AssetTag);
        }
    }

    // TagToLoadedAsset 제거
    for (const FGameplayTag& Tag : TagsToRemove)
    {
        AssetManager.TagToLoadedAsset.Remove(Tag);
    }

    // NameToTag에서 Value가 일치하는 항목 순회 삭제
    for (auto It = AssetManager.NameToTag.CreateIterator(); It; ++It)
    {
        if (TagsToRemove.Contains(It.Value()))
        {
            It.RemoveCurrent();
        }
    }
}

US1AssetData* US1AssetManager::GetLoadadAssetByTag(const FGameplayTag& DataTag)
{
    TObjectPtr<US1AssetData>* Found = LoadedAssetData.Find(DataTag);
    ensureAlwaysMsgf(Found, TEXT("Can't Find AssetData from DataTag [%s]."), *DataTag.ToString());
    return Found ? Found->Get() : nullptr;
}

void US1AssetManager::LoadAssetsToLabel(const FGameplayTag& Label, FAsyncLabelLoadCompletedDelegate CompletedDelegate)
{
    // PIE 재실행 시 이전 세션 데이터 초기화
    ReleaseAll();

    FPrimaryAssetType PrimaryAssetType(US1AssetData::StaticClass()->GetFName());
    TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);

    if (Handle.IsValid())
    {
        Handle->WaitUntilComplete(0.f, false);

        TArray<UObject*> LoadedObjects;
        Handle->GetLoadedAssets(LoadedObjects);

        for (UObject* Obj : LoadedObjects)
        {
            US1AssetData* AssetData = Cast<US1AssetData>(Obj);
            if (false == IsValid(AssetData))
            {
                continue;
            }

            const FGameplayTag& DataTag = AssetData->GetDataTag();
            ensureAlwaysMsgf(DataTag.IsValid(),
                TEXT("US1AssetData [%s] 에 DataTag가 설정되지 않음"), *AssetData->GetName());
        }

        // 모든 AssetData 등록 완료 후 Preload Label 에셋 비동기 로드
        LoadAsyncByLabel(Label, MoveTemp(CompletedDelegate));
    }
}
*/
#pragma endregion