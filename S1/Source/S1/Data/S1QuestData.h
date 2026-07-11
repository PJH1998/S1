// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1QuestData.generated.h"

class AS1QuestObject;

USTRUCT(BlueprintType)
struct FS1QuestEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AS1QuestObject> QuestClass;
};

USTRUCT(BlueprintType)
struct FS1QuestSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FS1QuestEntry> Quests;
};

UCLASS()
class S1_API US1QuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FS1QuestSet* FindByStageTag(FGameplayTag StageTag) const;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FS1QuestSet> StageQuests;
};
