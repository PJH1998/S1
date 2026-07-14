// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/S1HUD_Lobby.h"
#include "UI/Lobby/Logo/S1Logo.h"
#include "UI/Lobby/Custom/S1Customize.h"
#include "Components/WidgetSwitcher.h"

#include "System/S1UIManager.h"
#include "System/S1SoundManager.h"

US1HUD_Lobby::US1HUD_Lobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1HUD_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	Logo->OnClickedLogoDelegate.AddDynamic(this, &ThisClass::ShowCustomize);

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->PlayBGM(LobbyBGM, LobbyFadeInDuration);
	}
}

void US1HUD_Lobby::ShowCustomize()
{
	WidgetSwitcher_Root->SetActiveWidgetIndex(static_cast<int32>(HUD_LOBBY::CUSTOM));
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->FadeIn(3.f);
	}

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->PlayBGM(SelectedBGM, SelectedFadeInDuration);
	}
}
