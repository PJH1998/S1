// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "S1LogChannels.h"

void US1PlayerSet::InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag)
{
	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(AssetTag);
	if (!::IsValid(DTData)) { return; }

	UDataTable* DT = DTData->GetDataTable(TableTag);
	if (!::IsValid(DT)) { return; }

	TArray<FS1PlayerData*> Rows;
	DT->GetAllRows<FS1PlayerData>(TEXT(""), Rows);
	if (!Rows.IsValidIndex(0)) { return; }

	LOG(TEXT("MaxHealth: %f, BaseDamage: %f"), Rows[0]->MaxHealth, Rows[0]->BaseDamage);
}
