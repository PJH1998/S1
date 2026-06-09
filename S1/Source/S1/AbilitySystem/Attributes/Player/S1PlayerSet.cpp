// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1HitLagManager.h"
#include "S1GameplayTags.h"
#include "S1LogChannels.h"
#include "S1Define.h"

void US1PlayerSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute != GetHealthAttribute() || NewValue >= GetHealth())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (nullptr == ASC)
	{
		return;
	}

	if (false == ASC->HasMatchingGameplayTag(S1StateTags::State_Invincible))
	{
		return;
	}

	NewValue = GetHealth();

	UGameInstance* GI = ASC->GetWorld()->GetGameInstance();
	if (nullptr == GI)
	{
		return;
	}

	if (US1HitLagManager* HitLag = GI->GetSubsystem<US1HitLagManager>())
	{
		HitLag->TriggerHitLag(S1HitLagTags::HitLag_PerfectDodge);
	}
}

void US1PlayerSet::InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName)
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

	const FS1PlayerData* Row = nullptr;

	if (RowName == NAME_None)
	{
		TArray<FS1PlayerData*> Rows;
		DT->GetAllRows<FS1PlayerData>(TEXT(""), Rows);
		if (false == Rows.IsValidIndex(0))
		{
			return;
		}
		Row = Rows[0];
	}
	else
	{
		Row = DT->FindRow<FS1PlayerData>(RowName, TEXT(""));
		if (nullptr == Row)
		{
			return;
		}
	}

	CachedAssetTag = AssetTag;
	CachedTableTag = TableTag;

	FS1DefaultAttribute DefaultAttr;
	DefaultAttr.MaxHealth   = Row->MaxHealth;
	DefaultAttr.MaxStamina  = Row->MaxStamina;
	DefaultAttr.BaseDamage  = Row->BaseDamage;
	DefaultAttr.BaseDefense = Row->BaseDefense;
	InitDefaultAttribute(DefaultAttr);

	InitMaxXP(Row->MaxXP);
	InitCurrentXP(0.f);
}

void US1PlayerSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentXPAttribute())
	{
		SetCurrentXP(FMath::Max(0.f, GetCurrentXP()));

		while (GetCurrentXP() >= GetMaxXP())
		{
			LevelUp();
		}

		// Debug macro output. Remove after verifying XP pickup.
		D(FString::Printf(TEXT("XP %+0.f | Current: %.0f / %.0f | Level: %.0f"),
			Data.EvaluatedData.Magnitude,
			GetCurrentXP(),
			GetMaxXP(),
			GetLevel()));
	}
}

void US1PlayerSet::LevelUp()
{
	const float CarryOverXP = GetCurrentXP() - GetMaxXP();

	SetLevel(GetLevel() + 1.f);

	FName LevelRowName = FName(*FString::FromInt(FMath::RoundToInt(GetLevel())));
	InitAttributeFromTable(CachedAssetTag, CachedTableTag, LevelRowName);

	// InitAttributeFromTable이 CurrentXP를 0으로 리셋하므로 이후에 이월 XP 세팅
	InitCurrentXP(FMath::Max(0.f, CarryOverXP));

	LOG(TEXT("LEVEL UP!"));
}
