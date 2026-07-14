// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Customize.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Character/S1SelectCharacter.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Player/S1CharacterSelectController.h"
#include "System/S1SoundManager.h"

US1Customize::US1Customize(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Customize::NativeConstruct()
{
	Super::NativeConstruct();

	if (AS1SelectCharacter* PreviewActor = GetOwningPlayerPawn<AS1SelectCharacter>())
	{
		if (UTextureRenderTarget2D* RT = PreviewActor->GetPreviewRenderTarget())
		{
			Image_CharacterModel->SetBrushResourceObject(RT);
		}
	}

	if (Button_Start)
	{
		Button_Start->OnClicked.AddDynamic(this, &ThisClass::OnStartClicked);
	}
}

void US1Customize::OnStartClicked()
{
	if (AS1CharacterSelectController* Controller = GetOwningPlayer<AS1CharacterSelectController>())
	{
		Controller->RequestConfirm();
	}

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->StopBGM();
	}
}
