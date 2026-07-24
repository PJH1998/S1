// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1ItemManager.h"
#include "AbilitySystemComponent.h"
#include "Component/S1InventoryComponent.h"
#include "Data/S1DataTableData.h"
#include "Data/S1GameplayEffectData.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameplayEffect.h"
#include "Player/S1PlayerState.h"
#include "S1DataTableTypes.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"

bool US1ItemManager::ApplyPickup(AController* OwnerController, ES1DropItemType DropType, FGameplayTag ItemTag, FGameplayTag RarityTag, int32 Amount)
{
	UWorld* World = GetWorld();
	if (World == nullptr || World->GetAuthGameMode() == nullptr)
	{
		return false;
	}

	AS1PlayerState* PlayerState = GetS1PlayerState(OwnerController);
	if (false == IsValid(PlayerState) || Amount <= 0)
	{
		return false;
	}

	switch (DropType)
	{
	case ES1DropItemType::Gold:
		return ApplyGold(PlayerState, Amount);
	case ES1DropItemType::Exp:
		return ApplyExp(PlayerState, Amount);
	case ES1DropItemType::Item:
	default:
		return ApplyItem(PlayerState, ItemTag, Amount);
	}
}

const FS1ItemData* US1ItemManager::FindItemData(FGameplayTag ItemTag) const
{
	if (false == ItemTag.IsValid())
	{
		return nullptr;
	}

	US1DataTableData* DataTableData = US1AssetManager::GetAssetByTag<US1DataTableData>(S1AssetTags::Asset_DataTable);
	if (false == IsValid(DataTableData))
	{
		return nullptr;
	}

	UDataTable* ItemDataTable = DataTableData->GetDataTable(S1DataTableTags::DataTable_ItemData);
	if (false == IsValid(ItemDataTable))
	{
		return nullptr;
	}

	return ItemDataTable->FindRow<FS1ItemData>(ItemTag.GetTagName(), TEXT(""));
}

FGameplayTag US1ItemManager::FindWeaponItemTagByTypeAndRarity(ES1WeaponType WeaponType, FGameplayTag RarityTag) const
{
	if (ES1WeaponType::None == WeaponType || false == RarityTag.IsValid())
	{
		return FGameplayTag();
	}

	US1DataTableData* DataTableData = US1AssetManager::GetAssetByTag<US1DataTableData>(S1AssetTags::Asset_DataTable);
	if (false == IsValid(DataTableData))
	{
		return FGameplayTag();
	}

	UDataTable* ItemDataTable = DataTableData->GetDataTable(S1DataTableTags::DataTable_ItemData);
	if (false == IsValid(ItemDataTable))
	{
		return FGameplayTag();
	}

	// 행 이름이 곧 아이템 태그 — 계열당 등급별 무기는 1개라는 전제로 첫 매칭을 그대로 사용
	for (const TPair<FName, uint8*>& Row : ItemDataTable->GetRowMap())
	{
		const FS1ItemData* ItemData = reinterpret_cast<const FS1ItemData*>(Row.Value);
		if (ItemData == nullptr || ItemData->WeaponType != WeaponType || ItemData->Rarity != RarityTag)
		{
			continue;
		}

		return FGameplayTag::RequestGameplayTag(Row.Key, /*ErrorIfNotFound=*/false);
	}

	return FGameplayTag();
}

bool US1ItemManager::ApplyExp(AS1PlayerState* PlayerState, int32 Amount) const
{
	if (false == IsValid(PlayerState) || Amount <= 0)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
	if (false == IsValid(AbilitySystemComponent))
	{
		return false;
	}

	US1GameplayEffectData* GameplayEffectData = US1AssetManager::GetAssetByTag<US1GameplayEffectData>(S1AssetTags::Asset_GameplayEffect);
	if (false == IsValid(GameplayEffectData))
	{
		return false;
	}

	TSubclassOf<UGameplayEffect> GainXPClass = GameplayEffectData->FindEffectClassByTag(S1GameplayEffectTags::GameplayEffect_GainXP);
	if (GainXPClass == nullptr)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GainXPClass, 1.f, Context);
	if (false == Spec.IsValid())
	{
		return false;
	}

	Spec.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_XP, static_cast<float>(Amount));
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	return true;
}

bool US1ItemManager::ApplyGold(AS1PlayerState* PlayerState, int32 Amount) const
{
	if (false == IsValid(PlayerState) || Amount <= 0)
	{
		return false;
	}

	US1InventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent();
	if (false == IsValid(InventoryComponent))
	{
		return false;
	}

	return InventoryComponent->AddGold(Amount);
}

bool US1ItemManager::ApplyItem(AS1PlayerState* PlayerState, FGameplayTag ItemTag, int32 Amount) const
{
	if (false == IsValid(PlayerState) || Amount <= 0 || false == ItemTag.IsValid())
	{
		return false;
	}

	if (FindItemData(ItemTag) == nullptr)
	{
		return false;
	}

	US1InventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent();
	if (false == IsValid(InventoryComponent))
	{
		return false;
	}

	return InventoryComponent->AddItem(ItemTag, Amount);
}

AS1PlayerState* US1ItemManager::GetS1PlayerState(AController* OwnerController) const
{
	if (false == IsValid(OwnerController))
	{
		return nullptr;
	}

	return OwnerController->GetPlayerState<AS1PlayerState>();
}
