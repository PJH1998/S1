// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1HitLagManager.h"
#include "Data/S1DataTableData.h"
#include "System/S1AssetManager.h"
#include "S1DataTableTypes.h"
#include "S1GameplayTags.h"
#include "GameFramework/WorldSettings.h"
#include "S1LogChannels.h"

void US1HitLagManager::BuildCache()
{
	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(S1AssetTags::Asset_DataTable);
	if (false == ::IsValid(DTData))
	{
		LOG_ERROR(TEXT("HitLagManager: DataTableData not found"));
		return;
	}

	UDataTable* DT = DTData->GetDataTable(S1DataTableTags::DataTable_HitLagData);
	if (false == ::IsValid(DT))
	{
		LOG_ERROR(TEXT("HitLagManager: DT_HitLag not found"));
		return;
	}

	CachedConfigs.Reset();

	for (const FName& RowName : DT->GetRowNames())
	{
		const FS1HitLagRow* Row = DT->FindRow<FS1HitLagRow>(RowName, TEXT(""));
		if (nullptr == Row)
		{
			continue;
		}

		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(RowName);
		if (Tag.IsValid())
		{
			CachedConfigs.Emplace(Tag, *Row);
		}
	}
}

void US1HitLagManager::TriggerHitLag(FGameplayTag HitLagTag)
{
	const FS1HitLagRow* Row = CachedConfigs.Find(HitLagTag);
	if (nullptr == Row)
	{
		LOG_WARNING(TEXT("HitLagManager: No config for tag [%s]"), *HitLagTag.ToString());
		return;
	}

	UWorld* World = GetGameInstance()->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	// 진행 중인 HitLag 즉시 복원 후 새로 적용
	if (World->GetTimerManager().IsTimerActive(RestoreTimer))
	{
		World->GetTimerManager().ClearTimer(RestoreTimer);
		RestoreTimeDilation();
	}

	bCurrentExcludePlayer = Row->bExcludePlayer;
	World->GetWorldSettings()->TimeDilation = Row->SlowScale;

	if (Row->bExcludePlayer)
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->CustomTimeDilation = 1.f / Row->SlowScale;
			}
		}
	}

	// Duration은 실제 시간 기준 → 게임 타임 변환
	World->GetTimerManager().SetTimer(
		RestoreTimer,
		this,
		&ThisClass::RestoreTimeDilation,
		Row->Duration * Row->SlowScale,
		false
	);
}

void US1HitLagManager::RestoreTimeDilation()
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	World->GetWorldSettings()->TimeDilation = 1.f;

	if (false == bCurrentExcludePlayer)
	{
		return;
	}

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->CustomTimeDilation = 1.f;
		}
	}
}
