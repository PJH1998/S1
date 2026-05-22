// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Loading/S1LoadingGameMode.h"
#include "System/S1LevelManager.h"

void AS1LoadingGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	US1LevelManager::Get(this)->LoadResources();
}
