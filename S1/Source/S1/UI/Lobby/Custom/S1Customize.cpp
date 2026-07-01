// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Customize.h"
#include "Components/Image.h"
#include "Character/S1SelectCharacter.h"
#include "Engine/TextureRenderTarget2D.h"

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
}
