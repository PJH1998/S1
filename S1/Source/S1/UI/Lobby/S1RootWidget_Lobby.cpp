// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/S1RootWidget_Lobby.h"

US1RootWidget_Lobby::US1RootWidget_Lobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1RootWidget_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	SetCursorVisible(true);
}
