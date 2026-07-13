// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1HUD_Lobby.generated.h"

enum class HUD_LOBBY { LOGO, CUSTOM };

class UWidgetSwitcher;
class US1Logo;
class US1Customize;
class USoundBase;
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

private:
	UFUNCTION()
	void ShowCustomize();

	UFUNCTION()
	void OnLogoClickBegin();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_Root;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1Logo> Logo;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<USoundBase> LobbyBGM;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	float LobbyFadeInDuration = 1.5f;

	// Logo 클릭 즉시(화면 FadeOut 시작과 동시) Lobby BGM을 죽이는 데 걸리는 시간 — UI FadeOut 길이(현재 3초)와 맞추면 됨
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	float LobbyFadeOutDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<USoundBase> SelectedBGM;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	float SelectedFadeInDuration = 1.5f;
};
