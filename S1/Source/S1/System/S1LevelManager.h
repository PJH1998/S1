// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1LevelManager.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1LevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static US1LevelManager* Get(const UObject* WorldContextObject);

public:
	void			ChangeLevel(const FName& NextLevelName, const FGameplayTag& NextAssetLabel);

	void			LoadResources();

private:
	bool			bIsLoading = false;
	FName			LoadLevelName;
	FGameplayTag	LoadAssetLabel;
};
