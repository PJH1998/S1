// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1QuestManager.generated.h"

class AS1QuestObject;

// 레벨 내 활성 퀘스트 목록 — AS1QuestObject::BeginPlay/EndPlay에서 태그별로 등록/해제.
// 월드 시작 시 US1QuestData를 읽어 해당 스테이지의 퀘스트 액터를 자동 스폰(레벨에 수동 배치 불필요).
UCLASS()
class S1_API US1QuestManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void RegisterQuest(FGameplayTag Tag, AS1QuestObject* Quest);
	void UnregisterQuest(FGameplayTag Tag, AS1QuestObject* Quest);

	// 서버 전용: 해당 태그의 퀘스트 진행도를 올린다. 퍼즐 완료 등 BP에서 직접 호출하는 진입점.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddQuestProgress(FGameplayTag QuestTag, int32 Amount = 1);

private:
	// 레벨의 QuestAssetTag/QuestStageTag로 US1QuestData를 찾아 해당 스테이지의 퀘스트 액터를 전부 스폰(서버 전용).
	void SpawnQuestsFromAsset(FGameplayTag AssetTag, FGameplayTag StageTag);

private:
	TMap<FGameplayTag, TArray<TWeakObjectPtr<AS1QuestObject>>> ActiveQuests;
};
