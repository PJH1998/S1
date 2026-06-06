// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/S1Monster.h"
#include "Data/S1DataTableData.h"
#include "Data/S1GameplayEffectData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "S1GameplayTags.h"
#include "S1LogChannels.h"

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

	// XP 전달
	AActor* Killer = Data.EffectSpec.GetContext().GetInstigator();
	UAbilitySystemComponent* KillerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Killer);
	if (::IsValid(KillerASC))
	{
		US1GameplayEffectData* GEData = US1AssetManager::GetAssetByTag<US1GameplayEffectData>(S1AssetTags::Asset_GameplayEffect);
		if (::IsValid(GEData))
		{
			TSubclassOf<UGameplayEffect> GainXPClass = GEData->FindEffectClassByTag(S1GameplayEffectTags::GameplayEffect_GainXP);
			if (nullptr != GainXPClass)
			{
				FGameplayEffectContextHandle Context = KillerASC->MakeEffectContext();
				FGameplayEffectSpecHandle Spec = KillerASC->MakeOutgoingSpec(GainXPClass, 1.f, Context);
				if (Spec.IsValid())
				{
					Spec.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_XP, RewardXP);
					KillerASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
				}
			}
		}
	}

	// 사망 처리
	UAbilitySystemComponent* MonsterASC = GetOwningAbilitySystemComponent();
	if (nullptr == MonsterASC)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(MonsterASC->GetAvatarActor()))
	{
		Monster->NotifyDeath();
	}
}

void US1MonsterSet::InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName)
{
	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(AssetTag);
	if (false == ::IsValid(DTData))
	{
		return;
	}

	UDataTable* DT = DTData->GetDataTable(TableTag);
	if (false == ::IsValid(DT))
	{
		return;
	}

	const FS1MonsterData* Row = nullptr;

	if (RowName == NAME_None)
	{
		TArray<FS1MonsterData*> Rows;
		DT->GetAllRows<FS1MonsterData>(TEXT(""), Rows);
		if (false == Rows.IsValidIndex(0))
		{
			return;
		}
		Row = Rows[0];
	}
	else
	{
		Row = DT->FindRow<FS1MonsterData>(RowName, TEXT(""));
		if (nullptr == Row)
		{
			return;
		}
	}

	FS1DefaultAttribute DefaultAttr;
	DefaultAttr.MaxHealth = Row->MaxHealth;
	DefaultAttr.MaxStamina = Row->MaxStamina;
	DefaultAttr.BaseDamage = Row->BaseDamage;
	DefaultAttr.BaseDefense = Row->BaseDefense;
	InitDefaultAttribute(DefaultAttr);

	RewardXP = Row->RewardXP;
}