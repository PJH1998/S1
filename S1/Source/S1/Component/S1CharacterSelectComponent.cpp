// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1CharacterSelectComponent.h"

#include "Character/S1SelectCharacter.h"
#include "Character/Player/S1Player.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1DataTableData.h"
#include "Engine/DataTable.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/S1PlayerState.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1SoundManager.h"
#include "System/S1UIManager.h"
#include "Tags/S1GameplayTags.h"
#include "UI/S1RootWidget.h"

#include "S1LogChannels.h"

US1CharacterSelectComponent::US1CharacterSelectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void US1CharacterSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwner<APlayerController>();
	if (false == ::IsValid(PC) || false == PC->IsLocalController())
	{
		return;
	}

	//Test
	CharacterTagA = S1CharacterTag::Charcter_Select_Asna;
	CharacterTagB = S1CharacterTag::Charcter_Select_Kirito;

	SpawnLocalPreview();

	if (::IsValid(PreviewActor))
	{
		SelectCharacter(CharacterTagA);
	}
}

void US1CharacterSelectComponent::OnSelectA()
{
	SelectCharacter(CharacterTagA);
}

void US1CharacterSelectComponent::OnSelectB()
{
	SelectCharacter(CharacterTagB);
}

void US1CharacterSelectComponent::OnWeaponSelect(const FGameplayTag& WeaponTag)
{
	if (::IsValid(PreviewActor))
	{
		PreviewActor->ChangeWeapon(WeaponTag);
	}

	// 로컬 프리뷰 여부와 무관하게 항상 서버에 반영 — Start 확정 시 실제 Pawn 스폰에서 이 태그를 읽음
	ServerSetSelectedWeapon(WeaponTag);
}

void US1CharacterSelectComponent::RequestConfirm()
{
	OnConfirm();
}

void US1CharacterSelectComponent::OnConfirm()
{
	LOG(TEXT("CharacterSelect: Confirm — Selected [%s]"), *CurrentSelectedTag.ToString());

	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	ServerConfirmSelection();
}

void US1CharacterSelectComponent::ServerConfirmSelection_Implementation()
{
	APlayerController* PC = GetOwner<APlayerController>();
	AS1PlayerState* PS = ::IsValid(PC) ? PC->GetPlayerState<AS1PlayerState>() : nullptr;
	if (false == ::IsValid(PS))
	{
		LOG_WARNING(TEXT("CharacterSelect: ServerConfirmSelection — PlayerState invalid, aborting"));
		return;
	}

	US1CharacterSelectData* SelectData = US1AssetManager::GetAssetByTag<US1CharacterSelectData>(S1AssetTags::Asset_CharacterData);
	if (false == ::IsValid(SelectData))
	{
		LOG_WARNING(TEXT("CharacterSelect: ServerConfirmSelection — CharacterSelectData not loaded, aborting"));
		return;
	}

	const FS1SelectCharacterEntry* Entry = SelectData->FindEntryByTag(PS->GetSelectedCharacterTag());
	if (nullptr == Entry || nullptr == Entry->CharacterClass)
	{
		LOG_WARNING(TEXT("CharacterSelect: no CharacterClass for tag [%s]"), *PS->GetSelectedCharacterTag().ToString());
		return;
	}

	PS->SetLastSpawnPointTag(S1SpawnTags::Spawn_Main_Default);

	FTransform SpawnTransform = FTransform::Identity;
	if (US1DataTableData* DataTableData = US1AssetManager::GetAssetByTag<US1DataTableData>(S1AssetTags::Asset_DataTable))
	{
		if (UDataTable* SpawnTable = DataTableData->GetDataTable(S1DataTableTags::DataTable_SpawnPoint))
		{
			if (const FS1SpawnPointRow* Row = SpawnTable->FindRow<FS1SpawnPointRow>(PS->GetLastSpawnPointTag().GetTagName(), TEXT("")))
			{
				SpawnTransform = Row->SpawnTransform;
			}
			else
			{
				LOG_WARNING(TEXT("CharacterSelect: no SpawnPoint row for tag [%s] — spawning at world origin"), *PS->GetLastSpawnPointTag().ToString());
			}
		}
	}

	UWorld* World = GetWorld();
	if (false == ::IsValid(World))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AS1Player* NewPawn = World->SpawnActor<AS1Player>(Entry->CharacterClass, SpawnTransform, SpawnParams);
	if (false == ::IsValid(NewPawn))
	{
		LOG_WARNING(TEXT("CharacterSelect: SpawnActor<AS1Player> failed for class [%s]"), *GetNameSafe(Entry->CharacterClass.Get()));
		return;
	}

	PC->Possess(NewPawn);

	// SpawnLocalPreview()에서 프리뷰로 돌려둔 ViewTarget이 그대로 남아있으면 실제 Pawn을 possess해도 카메라가 안 바뀜(프리뷰는 곧 Destroy됨)
	PC->SetViewTarget(NewPawn);

	LOG(TEXT("CharacterSelect: ServerConfirmSelection — Possessed [%s] at [%s]"), *GetNameSafe(NewPawn), *SpawnTransform.GetLocation().ToString());
}

void US1CharacterSelectComponent::SelectCharacter(const FGameplayTag& Tag)
{
	if (false == Tag.IsValid())
	{
		return;
	}

	CurrentSelectedTag = Tag;
	UpdatePreview(Tag);
	ServerSetSelectedCharacter(Tag);
}

void US1CharacterSelectComponent::UpdatePreview(const FGameplayTag& Tag)
{
	if (false == ::IsValid(PreviewActor))
	{
		LOG_WARNING(TEXT("CharacterSelect: PreviewActor is not valid"));
		return;
	}

	PreviewActor->ChangeMesh(Tag);
}

void US1CharacterSelectComponent::SpawnLocalPreview()
{
	UWorld* World = GetWorld();
	if (false == ::IsValid(World))
	{
		return;
	}

	APlayerStart* BoothStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()));
	const FTransform SpawnTransform = ::IsValid(BoothStart) ? BoothStart->GetActorTransform() : FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewActor = World->SpawnActor<AS1SelectCharacter>(PreviewCharacterClass, SpawnTransform, SpawnParams);
	if (false == ::IsValid(PreviewActor))
	{
		LOG_WARNING(TEXT("CharacterSelect: failed to spawn local preview actor — check PreviewCharacterClass"));
		return;
	}

	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		PC->SetViewTarget(PreviewActor);
	}
}

void US1CharacterSelectComponent::HandleGameplayPawnPossessed()
{
	if (bGameplayUISwapped)
	{
		LOG(TEXT("CharacterSelect: HandleGameplayPawnPossessed — already swapped, skip"));
		return;
	}
	bGameplayUISwapped = true;

	LOG(TEXT("CharacterSelect: HandleGameplayPawnPossessed — swapping to gameplay UI"));

	if (::IsValid(PreviewActor))
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->StopBGM();
	}

	APlayerController* PC = GetOwner<APlayerController>();
	if (false == ::IsValid(PC))
	{
		return;
	}

	// SUBSYSTEM(T) 매크로(GetLocalPlayerSubsystem(this, ...))는 this가 ActorComponent라 LocalPlayer로 못 이어짐 — PC의 LocalPlayer로 직접 조회
	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	US1UIManager* UIManager = LocalPlayer ? LocalPlayer->GetSubsystem<US1UIManager>() : nullptr;
	if (nullptr == UIManager)
	{
		LOG_WARNING(TEXT("CharacterSelect: HandleGameplayPawnPossessed — US1UIManager not found (LocalPlayer=%s)"), *GetNameSafe(LocalPlayer));
		return;
	}

	// RootWidget 자체는 세션 내내 그대로 — HUD 패널 콘텐츠만 Lobby_HUD에서 Gameplay_HUD로 교체.
	// CanvasPanel_HUD 안에서만 자식이 바뀌는 거라(ClearChildren+새 위젯) RemoveFromParent 같은 지연 파괴가 없고,
	// Fade/Cursor도 같은 Root를 계속 쓰니 리셋 없이 자연스럽게 이어짐.
	US1RootWidget* Root = UIManager->GetRootWidget();
	if (nullptr == Root)
	{
		LOG_WARNING(TEXT("CharacterSelect: HandleGameplayPawnPossessed — GetRootWidget() was null"));
		return;
	}

	Root->SetUp_Panel(UI_TYPE::HUD, S1UITags::UI_Gameplay_HUD);
	Root->SetCursorVisible(false);

	// FadeIn은 여기서 걸지 않음 — US1Customize::OnSpawnSelectCharacter가 RequestConfirm() 직후 이미 걸어둠(Root가
	// 세션 내내 유지되니 이 함수가 나중에 실행돼도 안전). 여기서 또 걸면 bOnFade 가드에 막혀 무의미한 중복 호출.
}

void US1CharacterSelectComponent::ServerSetSelectedWeapon_Implementation(FGameplayTag Tag)
{
	APlayerController* PC = GetOwner<APlayerController>();
	AS1PlayerState* PS = ::IsValid(PC) ? PC->GetPlayerState<AS1PlayerState>() : nullptr;
	if (false == ::IsValid(PS))
	{
		return;
	}

	PS->SetSelectedWeaponTag(Tag);
}

void US1CharacterSelectComponent::ServerSetSelectedCharacter_Implementation(FGameplayTag Tag)
{
	APlayerController* PC = GetOwner<APlayerController>();
	AS1PlayerState* PS = ::IsValid(PC) ? PC->GetPlayerState<AS1PlayerState>() : nullptr;
	if (false == ::IsValid(PS))
	{
		return;
	}

	PS->SetSelectedCharacterTag(Tag);
}
