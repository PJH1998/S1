// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Character/S1Monster.h"
#include "Data/S1DataTableData.h"
#include "Engine/World.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1CombatFeedbackSubsystem.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"

void US1MonsterSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetHealthAttribute())
	{
		return;
	}

	UAbilitySystemComponent* MonsterASC = GetOwningAbilitySystemComponent();
	if (nullptr == MonsterASC)
	{
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(MonsterASC->GetAvatarActor());
	if (nullptr == Monster)
	{
		return;
	}

	if (Data.EvaluatedData.Magnitude < 0.f)
	{
		FVector DamageLocation = Monster->GetActorLocation();
		if (const FHitResult* HitResult = Data.EffectSpec.GetContext().GetHitResult())
		{
			if (HitResult->ImpactPoint.IsNearlyZero() == false)
			{
				DamageLocation = HitResult->ImpactPoint;
			}
			else if (HitResult->Location.IsNearlyZero() == false)
			{
				DamageLocation = HitResult->Location;
			}
		}

		if (UWorld* World = Monster->GetWorld())
		{
			if (World->GetNetMode() != NM_DedicatedServer)
			{
				if (US1CombatFeedbackSubsystem* CombatFeedback = World->GetSubsystem<US1CombatFeedbackSubsystem>())
				{
					const int32 Damage = FMath::RoundToInt(FMath::Abs(Data.EvaluatedData.Magnitude));
					CombatFeedback->ShowDamageNumber(Damage, DamageLocation);
				}
			}
		}
	}

	if (GetHealth() > 0.f)
	{
		return;
	}

	// 사망 처리
	Monster->NotifyDeath();
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

	if (UAbilitySystemComponent* MonsterASC = GetOwningAbilitySystemComponent())
	{
		if (AS1Monster* Monster = Cast<AS1Monster>(MonsterASC->GetAvatarActor()))
		{
			Monster->SetMonsterName(Row->MonsterName);
		}
	}

	RewardXP = Row->RewardXP;
}