// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1HUD_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1HUD_Lobby : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1HUD_Lobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

};
