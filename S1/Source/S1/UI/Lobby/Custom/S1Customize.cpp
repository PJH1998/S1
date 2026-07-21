// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Customize.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Character/S1SelectCharacter.h"
#include "Component/S1CharacterSelectComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Player/S1PlayerController.h"
#include "System/S1SoundManager.h"
#include "TimerManager.h"
#include "System/S1UIManager.h"
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
	AS1PlayerController* Controller = GetOwningPlayer<AS1PlayerController>();
	US1CharacterSelectComponent* Comp = Controller ? Controller->GetCharacterSelectComponent() : nullptr;
	AS1SelectCharacter* PreviewActor = Comp ? Comp->GetPreviewActor() : nullptr;
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
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	UE_LOG(LogTemp, Warning, TEXT("MouseCursor : %d"), PC->bShowMouseCursor);

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->FadeOut(1.5f, [this]()
			{
				this->OnSpawnSelectCharacter();
			});
	}

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->PlaySoundByTag(S1SoundTags::Sound_UI_Confirm);
		SoundManager->StopBGM(1.5f);
	}
}

void US1Customize::OnSpawnSelectCharacter()
{
	// Root 위젯(과 그 안의 Fade)이 세션 내내 유지되고 HUD 패널 콘텐츠만 바뀌는 구조로 바뀌어서,
	// 이 위젯(S1HUD_Lobby 하위)이 나중에 SetUp_Panel로 교체되며 파괴되더라도 Fade 애니메이션은 Root에 남아
	// 끊기지 않고 계속 재생됨 — RequestConfirm() 직후 바로 FadeIn을 걸어도 안전함.
	if (AS1PlayerController* Controller = GetOwningPlayer<AS1PlayerController>())
	{
		if (US1CharacterSelectComponent* Comp = Controller->GetCharacterSelectComponent())
		{
			Comp->RequestConfirm();
		}
	}

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->FadeIn(1.5f);
	}
}
