// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Character/S1Monster.h"

#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"

void US1MonsterSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetHealthAttribute())
	{
		return;
	}

	if (GetHealth() > 0.f)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(AbilitySystemComponent->GetAvatarActor()))
	{
		Monster->NotifyDeath();
	}
}

void US1MonsterSet::InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName)
{
	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(AssetTag);
	if (!::IsValid(DTData)) { return; }

	UDataTable* DT = DTData->GetDataTable(TableTag);
	if (!::IsValid(DT)) { return; }

	if (RowName == NAME_None) { return; }

	const FS1MonsterData* Data = {};

	Data = DT->FindRow<FS1MonsterData>(RowName, TEXT(""));
	if (Data == nullptr) { return; }

	InitHealth(Data->MaxHealth);
	InitMaxHealth(Data->MaxHealth);
	InitBaseDamage(Data->BaseDamage);
	//InitBaseDefense(Data->BaseDamage);
}