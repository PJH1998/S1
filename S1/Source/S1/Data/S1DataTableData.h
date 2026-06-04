// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1DataTableData.generated.h"

class UDataTable;

UCLASS()
class S1_API US1DataTableData : public UDataAsset
{
	GENERATED_BODY()

public:
	UDataTable* GetDataTable(const FGameplayTag& TableTag) const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	TMap<FGameplayTag, TObjectPtr<UDataTable>> DataTables;
};
