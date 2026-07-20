// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gameplay/S1RespawnPrompt.h"

#include "Components/Button.h"
#include "Player/S1PlayerController.h"

void US1RespawnPrompt::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Respawn)
	{
		Button_Respawn->OnClicked.AddDynamic(this, &ThisClass::HandleRespawnButtonClicked);
	}
}

void US1RespawnPrompt::HandleRespawnButtonClicked()
{
	if (AS1PlayerController* PC = GetOwningPlayer<AS1PlayerController>())
	{
		PC->TryRespawn();
	}
}
