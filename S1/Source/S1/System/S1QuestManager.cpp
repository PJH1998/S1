// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1QuestManager.h"

#include "Data/S1QuestData.h"
#include "Engine/World.h"
#include "Quest/S1QuestObject.h"
#include "System/S1AssetManager.h"
#include "System/S1WorldSettings.h"

void US1QuestManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// CurrentCount는 복제되는 서버 권위 상태 — 클라가 직접 스폰하면 리플리케이션 액터와 중복되므로 서버에서만 스폰.
	if (nullptr == InWorld.GetAuthGameMode())
	{
		return;
	}

	AS1WorldSettings* WS = Cast<AS1WorldSettings>(InWorld.GetWorldSettings());
	if (false == IsValid(WS) || false == WS->QuestAssetTag.IsValid() || false == WS->QuestStageTag.IsValid())
	{
		return;
	}

	SpawnQuestsFromAsset(WS->QuestAssetTag, WS->QuestStageTag);
}

void US1QuestManager::RegisterQuest(FGameplayTag Tag, AS1QuestObject* Quest)
{
	if (false == Tag.IsValid() || false == ::IsValid(Quest))
	{
		return;
	}

	ActiveQuests.FindOrAdd(Tag).AddUnique(Quest);
}

void US1QuestManager::UnregisterQuest(FGameplayTag Tag, AS1QuestObject* Quest)
{
	if (TArray<TWeakObjectPtr<AS1QuestObject>>* Quests = ActiveQuests.Find(Tag))
	{
		Quests->RemoveSwap(Quest);
	}
}

void US1QuestManager::AddQuestProgress(FGameplayTag QuestTag, int32 Amount)
{
	const TArray<TWeakObjectPtr<AS1QuestObject>>* Quests = ActiveQuests.Find(QuestTag);
	if (nullptr == Quests)
	{
		return;
	}

	for (const TWeakObjectPtr<AS1QuestObject>& WeakQuest : *Quests)
	{
		if (AS1QuestObject* Quest = WeakQuest.Get())
		{
			Quest->AddProgress(Amount);
		}
	}
}

void US1QuestManager::SpawnQuestsFromAsset(FGameplayTag AssetTag, FGameplayTag StageTag)
{
	const US1QuestData* QuestData = US1AssetManager::GetAssetByTag<US1QuestData>(AssetTag);
	const FS1QuestSet* QuestSet = IsValid(QuestData) ? QuestData->FindByStageTag(StageTag) : nullptr;
	if (nullptr == QuestSet)
	{
		return;
	}

	for (const FS1QuestEntry& Entry : QuestSet->Quests)
	{
		if (Entry.QuestClass)
		{
			GetWorld()->SpawnActor<AS1QuestObject>(Entry.QuestClass);
		}
	}
}
