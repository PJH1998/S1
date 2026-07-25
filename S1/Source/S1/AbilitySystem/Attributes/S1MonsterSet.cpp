// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "AbilitySystem/S1HitReactLibrary.h"
#include "Character/S1Monster.h"
#include "Data/S1DataTableData.h"
#include "Engine/World.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
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

		// 데미지 폰트는 클라이언트 HP바(S1MonsterHPBar::NativeTick)에서 HP 감소를 감지하여 출력
		// — PostGameplayEffectExecute는 서버 전용이므로 여기서 직접 호출하지 않음

		// 피격음 — 서버 전용 훅이라 멀티캐스트로 전 클라 3D 재생(플레이어와 동일하게 HP 실제 감소 시에만).
		// 보스 가드 정면 차단은 US1BossSet가 Super 호출 전에 return하므로 여기까지 안 옴 → 가드음은 BossSet가 담당.
		const ES1HitReactType HitType = S1HitReactLibrary::ParseHitTypeFromSpec(Data.EffectSpec);
		const FGameplayTag SoundBaseTag = S1HitReactLibrary::FindSoundBaseTag(Data.EffectSpec);
		Monster->MulticastPlayHitSound(SoundBaseTag, HitType, DamageLocation);

		// 공격한 플레이어의 얼티밋 게이지를 실제 적용된(방어력 반영) 데미지만큼 증가
		if (UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent())
		{
			if (US1PlayerSet* SourcePlayerSet = const_cast<US1PlayerSet*>(Cast<US1PlayerSet>(SourceASC->GetAttributeSet(US1PlayerSet::StaticClass()))))
			{
				SourcePlayerSet->AddUltimateGauge(-Data.EvaluatedData.Magnitude);
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