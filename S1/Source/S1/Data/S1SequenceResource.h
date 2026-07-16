// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1SequenceResource.generated.h"

class ULevelSequence;

USTRUCT(BlueprintType)
struct FS1Sequence
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SequenceTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelSequence> Sequence = nullptr;
};

UCLASS()
class S1_API US1SequenceResource : public UDataAsset
{
	GENERATED_BODY()

public:
	ULevelSequence* FindSequenceByTag(const FGameplayTag& SequenceTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sequence")
	TArray<FS1Sequence> Sequences;
};