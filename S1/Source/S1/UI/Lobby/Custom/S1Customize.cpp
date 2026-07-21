// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Customize.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Character/S1SelectCharacter.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Player/S1CharacterSelectController.h"
#include "System/S1SoundManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Tags/S1GameplayTags.h"

US1Customize::US1Customize(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Customize::NativeConstruct()
{
	Super::NativeConstruct();

	BindPreviewRenderTarget();

	// Controller::BeginPlay(이 위젯 생성)가 Pawn::BeginPlay(인스턴스 RT 생성)보다 먼저 돌 수 있어 — 그 경우 위
	// BindPreviewRenderTarget()은 아직 없는 인스턴스 대신 템플릿을 잡음. 1틱 뒤 한 번 더 시도해 최신 RT로 교체
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			BindPreviewRenderTarget();
		}));
	}

	if (Button_Start)
	{
		Button_Start->OnClicked.AddDynamic(this, &ThisClass::OnStartClicked);
	}
}

void US1Customize::BindPreviewRenderTarget()
{
	AS1SelectCharacter* PreviewActor = GetOwningPlayerPawn<AS1SelectCharacter>();
	if (false == ::IsValid(PreviewActor))
	{
		return;
	}

	if (UTextureRenderTarget2D* RT = PreviewActor->GetPreviewRenderTarget())
	{
		Image_CharacterModel->SetBrushResourceObject(RT);
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
		SoundManager->PlaySoundByTag(S1SoundTags::Sound_UI_Confirm);
		SoundManager->StopBGM();
	}
}
