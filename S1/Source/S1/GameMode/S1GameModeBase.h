// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "S1GameModeBase.generated.h"

/**
 *
 */
UCLASS()
class S1_API AS1GameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	AS1GameModeBase();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};
