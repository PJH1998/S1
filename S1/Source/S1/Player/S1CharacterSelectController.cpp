// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/S1CharacterSelectController.h"

#include "S1Define.h"
#include "Character/S1SelectCharacter.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1LevelManager.h"

#include "S1LogChannels.h"

void AS1CharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsLocalController())
	{
		return;
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


	InputComponent->BindKey(EKeys::NumPadFour, IE_Pressed, this, &ThisClass::OnWeaponSelectA);
	InputComponent->BindKey(EKeys::NumPadFive, IE_Pressed, this, &ThisClass::OnWeaponSelectB);
}

void AS1CharacterSelectController::OnSelectA()
{
	SelectCharacter(CharacterTagA);
}

void AS1CharacterSelectController::OnSelectB()
{
	SelectCharacter(CharacterTagB);
}

void AS1CharacterSelectController::OnWeaponSelectA()
{
	PreviewActor->ChangeWeapon(S1ItemTags::Item_Weapon_RPR00);
}

void AS1CharacterSelectController::OnWeaponSelectB()
{
	PreviewActor->ChangeWeapon(S1ItemTags::Item_Weapon_SWD00);
}

void AS1CharacterSelectController::OnConfirm()
{
	LOG(TEXT("CharacterSelect: Confirm — Selected [%s]"), *CurrentSelectedTag.ToString());

	ServerConfirmSelection();
}

void AS1CharacterSelectController::ServerConfirmSelection_Implementation()
{
	if (US1LevelManager* LM = US1LevelManager::Get(this))
	{
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
