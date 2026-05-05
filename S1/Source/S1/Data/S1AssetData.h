// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1AssetData.generated.h"

USTRUCT()
struct FAssetEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AssetTag;

	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> AssetLabels;
};

USTRUCT()
struct FAssetSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAssetEntry> AssetEntries;
};

UCLASS()
class S1_API US1AssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void		PreSave(FObjectPreSaveContext ObjectSaveContext) override;

public:
	FSoftObjectPath		GetAssetPathByTag(const FGameplayTag& AssetTag);
	const FAssetSet&	GetAssetSetByLabel(const FGameplayTag& Label);
	const FAssetSet*	FindAssetSetByLabel(const FGameplayTag& Label) const;  // nullptr 반환 (크래시 없음)
	const FGameplayTag& GetDataTag();

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DataTag;

	// ��Ȯ�� Tag�� Path ã��
	UPROPERTY()
	TMap<FGameplayTag, FSoftObjectPath> AssetTagToPath;

	// �ش� Label (Tag)�� ������ Asset�� ã��
	UPROPERTY()
	TMap<FGameplayTag, FAssetSet> AssetLabelToSet;
};
