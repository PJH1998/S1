// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "S1LogChannels.h"

void US1PlayerSet::InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName)
{
	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(AssetTag);
	if (!::IsValid(DTData)) { return; }

	UDataTable* DT = DTData->GetDataTable(TableTag);
	if (!::IsValid(DT)) { return; }

	const FS1PlayerData* Row = nullptr;

	if (RowName == NAME_None)
	{
		TArray<FS1PlayerData*> Rows;
		DT->GetAllRows<FS1PlayerData>(TEXT(""), Rows);
		if (!Rows.IsValidIndex(0)) { return; }
		Row = Rows[0];
	}
	else
	{
		Row = DT->FindRow<FS1PlayerData>(RowName, TEXT(""));
		if (nullptr == Row) { return; }
	}

	LOG(TEXT("MaxHealth: %f, BaseDamage: %f"), Row->MaxHealth, Row->BaseDamage);
}
