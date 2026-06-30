// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/S1CharacterSelectController.h"

#include "S1Define.h"
#include "Data/S1CharacterSelectData.h"
#include "Character/Player/S1Player.h"
#include "Player/S1PlayerState.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1LevelManager.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

#include "S1LogChannels.h"

void AS1CharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsLocalController())
	{
		return;
	}

	if (ACameraActor* SelectCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass())))
	{
		SetViewTarget(SelectCamera);
	}

	if (US1CharacterSelectData* CharacterData = US1AssetManager::GetAssetByTag<US1CharacterSelectData>(S1AssetTags::Asset_CharacterData))
	{
		SelectData = CharacterData;
	}

	PreviewTransform = FTransform(FVector::ZeroVector);


	//Test
	CharacterTagA = S1CharacterTag::Charcter_Select_Asna;
	CharacterTagB = S1CharacterTag::Charcter_Select_Kirito;

	// A를 기본 선택으로 표시
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

void AS1CharacterSelectController::OnConfirm()
{
	LOG(TEXT("CharacterSelect: Confirm — Selected [%s]"), *CurrentSelectedTag.ToString());

	// travel은 서버 권위에서만 — 클라 입력을 서버로 라우팅
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
	if (false == ::IsValid(SelectData))
	{
		LOG_WARNING(TEXT("CharacterSelect: SelectData is not set"));
		return;
	}

	UClass* CharacterClass = SelectData->GetCharacterClass(Tag);
	if (nullptr == CharacterClass)
	{
		LOG_WARNING(TEXT("CharacterSelect: No CharacterClass for Tag [%s]"), *Tag.ToString());
		return;
	}

	if (::IsValid(PreviewActor))
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewActor = GetWorld()->SpawnActor<AS1Player>(CharacterClass, PreviewTransform, SpawnParams);
	if (false == ::IsValid(PreviewActor))
	{
		return;
	}

	// 리슨 서버에서 다른 클라로 복제 차단 — 순수 로컬 비주얼
	PreviewActor->SetReplicates(false);
	PreviewActor->SetActorEnableCollision(false);
	PreviewActor->SetActorTickEnabled(false);
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
