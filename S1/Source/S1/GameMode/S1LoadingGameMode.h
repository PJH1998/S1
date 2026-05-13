// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/S1GameModeBase.h"
#include "S1LoadingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class S1_API AS1LoadingGameMode : public AS1GameModeBase
{
	GENERATED_BODY()

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
