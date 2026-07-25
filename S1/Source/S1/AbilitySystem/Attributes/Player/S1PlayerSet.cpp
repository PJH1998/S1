// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1SoundManager.h"
#include "AbilitySystem/S1HitReactLibrary.h"
#include "Character/Player/S1Player.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"
#include "S1Define.h"

void US1PlayerSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentUltimateGaugeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxUltimateGauge());
		return;
	}

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

	// 무적(회피) 중 데미지 무효화 — 퍼펙트닷지 HitLag(슬로우모션)은 제거됨
	NewValue = GetHealth();
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

	InitMaxUltimateGauge(Row->MaxUltimateGauge);
	InitCurrentUltimateGauge(0.f);
	UpdateCanUltimateTag();
}

void US1PlayerSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 피격 리액션(GA_Hit) 트리거는 여기서(서버), 사운드는 MulticastPlayHitSound로 전 클라 재생 — HP 감소 시에만.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && Data.EvaluatedData.Magnitude < 0.f)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

		// PreAttributeChange가 Health 값 자체는 클램프해도 Magnitude는 그대로라 여기까지 들어옴 — 무적 중엔 리액션/사운드도 스킵
		if (IsValid(ASC) && ASC->HasMatchingGameplayTag(S1StateTags::State_Invincible))
		{
			return;
		}

		AActor* Avatar = IsValid(ASC) ? ASC->GetAvatarActor() : nullptr;

		if (IsValid(Avatar) && Avatar->HasAuthority())
		{
			const ES1HitReactType HitType = S1HitReactLibrary::ParseHitTypeFromSpec(Data.EffectSpec);
			const FGameplayTag HitTypeTag = S1HitReactLibrary::HitReactTypeToTag(HitType);

			if (HitTypeTag.IsValid())
			{
				AActor* Attacker = Data.EffectSpec.GetContext().GetEffectCauser();
				if (nullptr == Attacker)
				{
					Attacker = Data.EffectSpec.GetContext().GetInstigator();
				}

				// HP 0 이하면 Hit 대신 Death GA를 트리거(둘은 상호 배타적) — 원래 피격 타입과 무관하게 GA_Death 하나만 실행(애니메이션 제약).
				const bool bIsDead = GetHealth() <= 0.f;
				const FGameplayTag TriggerTag = bIsDead ? S1HitType::HitType_Death : HitTypeTag;

				if (TriggerTag.IsValid())
				{
					FGameplayEventData Payload;
					Payload.EventTag   = TriggerTag;
					Payload.Instigator = Attacker;
					Payload.Target     = Avatar;
					ASC->HandleGameplayEvent(TriggerTag, &Payload);
				}

				// 피격음 — PostGameplayEffectExecute는 서버 전용이라 직접 재생하면 데디/리모트 클라에서 안 들림.
				// AS1Character 멀티캐스트로 전 클라 3D 재생. HP 0 이하(사망)여도 피격음 자체는 재생.
				const FGameplayTag SoundBaseTag = S1HitReactLibrary::FindSoundBaseTag(Data.EffectSpec);
				FVector HitLocation = Avatar->GetActorLocation();
				if (const FHitResult* HitResult = Data.EffectSpec.GetContext().GetHitResult())
				{
					if (false == HitResult->ImpactPoint.IsNearlyZero())
					{
						HitLocation = HitResult->ImpactPoint;
					}
				}

				if (AS1Player* Player = Cast<AS1Player>(Avatar))
				{
					Player->MulticastPlayHitSound(SoundBaseTag, HitType, HitLocation);
				}
			}
		}
	}

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

void US1PlayerSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(US1PlayerSet, CurrentXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1PlayerSet, MaxXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1PlayerSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1PlayerSet, CurrentUltimateGauge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1PlayerSet, MaxUltimateGauge, COND_None, REPNOTIFY_Always);
}

void US1PlayerSet::OnRep_CurrentXP(const FGameplayAttributeData& OldCurrentXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1PlayerSet, CurrentXP, OldCurrentXP);
}

void US1PlayerSet::OnRep_MaxXP(const FGameplayAttributeData& OldMaxXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1PlayerSet, MaxXP, OldMaxXP);
}

void US1PlayerSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1PlayerSet, Level, OldLevel);
}

void US1PlayerSet::OnRep_CurrentUltimateGauge(const FGameplayAttributeData& OldCurrentUltimateGauge)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1PlayerSet, CurrentUltimateGauge, OldCurrentUltimateGauge);
}

void US1PlayerSet::OnRep_MaxUltimateGauge(const FGameplayAttributeData& OldMaxUltimateGauge)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1PlayerSet, MaxUltimateGauge, OldMaxUltimateGauge);
}

void US1PlayerSet::AddUltimateGauge(float Amount)
{
	float IncreaseGauge = Amount * GaugeMagnification;

	const float NewGauge = FMath::Clamp(GetCurrentUltimateGauge() + IncreaseGauge, 0.f, GetMaxUltimateGauge());
	SetCurrentUltimateGauge(NewGauge);
	UpdateCanUltimateTag();
}

void US1PlayerSet::ResetUltimateGauge()
{
	SetCurrentUltimateGauge(0.f);
	UpdateCanUltimateTag();
}

void US1PlayerSet::UpdateCanUltimateTag()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (nullptr == ASC)
	{
		return;
	}

	if (GetCurrentUltimateGauge() >= GetMaxUltimateGauge())
	{
		ASC->AddLooseGameplayTag(S1StateTags::State_CanUltimate, 1, EGameplayTagReplicationState::TagOnly);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(S1StateTags::State_CanUltimate, 1, EGameplayTagReplicationState::TagOnly);
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

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (AS1Player* Player = ASC ? Cast<AS1Player>(ASC->GetAvatarActor()) : nullptr)
	{
		Player->MulticastPlayLevelUpPresentation();
	}

	LOG(TEXT("LEVEL UP!"));
}
