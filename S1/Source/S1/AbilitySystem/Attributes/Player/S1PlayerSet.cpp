// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "System/S1HitLagManager.h"
#include "System/S1SoundManager.h"
#include "AbilitySystem/S1HitReactLibrary.h"
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

	InitMaxUltimateGauge(Row->MaxUltimateGauge);
	InitCurrentUltimateGauge(0.f);
	UpdateCanUltimateTag();
}

void US1PlayerSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(
			TEXT("[PlayerSet] PostGameplayEffectExecute 호출됨 — Attribute: %s, Magnitude: %.1f"),
			*Data.EvaluatedData.Attribute.GetName(), Data.EvaluatedData.Magnitude));
	}

	// 피격 리액션(GA_Hit) 트리거 + 사운드 — ReactComponent 대신 여기서 처리(HP 감소 시에만 정확히 발동).
	// ⚠️ 간단 테스트용: PostGameplayEffectExecute는 서버 전용이라 사운드는 서버(리슨 서버 호스트)에서만 들림 —
	// 리모트 클라 전파 여부는 아직 검증 안 됨.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && Data.EvaluatedData.Magnitude < 0.f)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

		// PreAttributeChange가 Health 값 자체는 클램프해도 Magnitude는 그대로라 여기까지 들어옴 — 무적 중엔 리액션/사운드도 스킵
		if (IsValid(ASC) && ASC->HasMatchingGameplayTag(S1StateTags::State_Invincible))
		{
			return;
		}

		AActor* Avatar = IsValid(ASC) ? ASC->GetAvatarActor() : nullptr;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[PlayerSet] PostGameplayEffectExecute 진입 (HP 감소 감지)"));
		}

		LOG(TEXT("[PlayerSet] HP -%.1f 감지 — Avatar: %s, NetMode: %d, HasAuthority: %s"),
			-Data.EvaluatedData.Magnitude,
			IsValid(Avatar) ? *Avatar->GetName() : TEXT("None"),
			Avatar ? (int32)Avatar->GetNetMode() : -1,
			(IsValid(Avatar) && Avatar->HasAuthority()) ? TEXT("true") : TEXT("false"));

		if (IsValid(Avatar) && Avatar->HasAuthority())
		{
			const ES1HitReactType HitType = S1HitReactLibrary::ParseHitTypeFromSpec(Data.EffectSpec);
			const FGameplayTag HitTypeTag = S1HitReactLibrary::HitReactTypeToTag(HitType);

			LOG(TEXT("[PlayerSet] HitType: %d, HitTypeTag: %s"), (int32)HitType, *HitTypeTag.ToString());

			if (HitTypeTag.IsValid())
			{
				AActor* Attacker = Data.EffectSpec.GetContext().GetEffectCauser();
				if (nullptr == Attacker)
				{
					Attacker = Data.EffectSpec.GetContext().GetInstigator();
				}

				FGameplayEventData Payload;
				Payload.EventTag   = HitTypeTag;
				Payload.Instigator = Attacker;
				Payload.Target     = Avatar;
				ASC->HandleGameplayEvent(HitTypeTag, &Payload);

				const FGameplayTag SoundBaseTag = S1HitReactLibrary::FindSoundBaseTag(Data.EffectSpec);
				US1SoundManager* SoundManager = Avatar->GetWorld()->GetSubsystem<US1SoundManager>();

				LOG(TEXT("[PlayerSet] SoundBaseTag: %s, SoundManager: %s"),
					*SoundBaseTag.ToString(),
					IsValid(SoundManager) ? TEXT("Valid") : TEXT("nullptr"));

				if (SoundManager)
				{
					SoundManager->PlayHitSound(SoundBaseTag, HitType);
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
	const float NewGauge = FMath::Clamp(GetCurrentUltimateGauge() + Amount, 0.f, GetMaxUltimateGauge());
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

	LOG(TEXT("LEVEL UP!"));
}
