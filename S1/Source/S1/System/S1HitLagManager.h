// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1DataTableTypes.h"
#include "S1HitLagManager.generated.h"

UCLASS()
class S1_API US1HitLagManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void BuildCache();
	void TriggerHitLag(FGameplayTag HitLagTag);
	void TriggerHitLagManual(FGameplayTag HitLagTag);
	void ReleaseHitLag();

private:
	void ApplyTimeDilation(const FS1HitLagRow& Row);
	void RestoreTimeDilation();

	TMap<FGameplayTag, FS1HitLagRow> CachedConfigs;
	FTimerHandle RestoreTimer;
	bool bCurrentExcludePlayer = false;
};
