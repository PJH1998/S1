// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1RootWidget.h"
#include "S1RootWidget_Lobby.generated.h"

/**
 *
 */
UCLASS()
class S1_API US1RootWidget_Lobby : public US1RootWidget
{
	GENERATED_BODY()

public:
	US1RootWidget_Lobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	virtual void NativeConstruct() override;
};
