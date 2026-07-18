// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/S1CharacterSelectController.h"

#include "S1Define.h"
#include "Character/S1SelectCharacter.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1LevelManager.h"
#include "System/S1UIManager.h"

#include "S1LogChannels.h"

void AS1CharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsLocalController())
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->Create_RootUI(this, S1UITags::UI_Root_Lobby);
	}

	PreviewActor = Cast<AS1SelectCharacter>(GetPawn());
	if (false == ::IsValid(PreviewActor))
	{
		LOG_WARNING(TEXT("CharacterSelect: AS1SelectCharacter not found — check SelectMap GameMode DefaultPawnClass"));
		return;
	}

	//Test
	CharacterTagA = S1CharacterTag::Charcter_Select_Asna;
	CharacterTagB = S1CharacterTag::Charcter_Select_Kirito;

	SelectCharacter(CharacterTagA);
}

void AS1CharacterSelectController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, this, &ThisClass::OnSelectA);
	InputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, this, &ThisClass::OnSelectB);
	InputComponent->BindKey(EKeys::NumPadThree, IE_Pressed, this, &ThisClass::OnConfirm);
}

void AS1CharacterSelectController::OnSelectA()
{
	SelectCharacter(CharacterTagA);
}

void AS1CharacterSelectController::OnSelectB()
{
	SelectCharacter(CharacterTagB);
}

void AS1CharacterSelectController::OnWeaponSelect(const FGameplayTag& WeaponTag)
{
	if (false == ::IsValid(PreviewActor))
	{
		return;
	}

	PreviewActor->ChangeWeapon(WeaponTag);
}

void AS1CharacterSelectController::RequestConfirm()
{
	OnConfirm();
}

void AS1CharacterSelectController::OnConfirm()
{
	LOG(TEXT("CharacterSelect: Confirm — Selected [%s]"), *CurrentSelectedTag.ToString());

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	ServerConfirmSelection();
}

void AS1CharacterSelectController::ServerConfirmSelection_Implementation()
{
	if (US1LevelManager* LM = US1LevelManager::Get(this))
	{
		PreviewActor->StopPreviewCapture();
		LM->ChangeLevel(S1Levels::MainMap, FGameplayTag::EmptyTag);
	}
}

void AS1CharacterSelectController::SelectCharacter(const FGameplayTag& Tag)
{
	if (false == Tag.IsValid())
	{
		return;
	}

	CurrentSelectedTag = Tag;
	UpdatePreview(Tag);
	ServerSetSelectedCharacter(Tag);
}

void AS1CharacterSelectController::UpdatePreview(const FGameplayTag& Tag)
{
	if (false == ::IsValid(PreviewActor))
	{
		LOG_WARNING(TEXT("CharacterSelect: PreviewActor is not valid"));
		return;
	}

	PreviewActor->ChangeMesh(Tag);
}

void AS1CharacterSelectController::ServerSetSelectedCharacter_Implementation(FGameplayTag Tag)
{
	AS1PlayerState* PS = GetPlayerState<AS1PlayerState>();
	if (false == ::IsValid(PS))
	{
		return;
	}

	PS->SetSelectedCharacterTag(Tag);
}
