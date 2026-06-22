// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1CombatFeedbackSubsystem.h"

#include "Data/S1EffectData.h"
#include "Effect/NiagaraEffect/S1DamageNumberEffect.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

void US1CombatFeedbackSubsystem::ShowDamageNumber(int32 Damage, const FVector& HitLocation) const
{
	if (Damage < 0)
	{
		Damage = FMath::Abs(Damage);
	}

	US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(S1AssetTags::Asset_Effect);
	if (EffectData == nullptr)
	{
		return;
	}

	const TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(S1EffectTags::Effect_DamageNumber);
	if (EffectClass == nullptr)
	{
		return;
	}

	const AS1DamageNumberEffect* DamageNumberEffect = Cast<AS1DamageNumberEffect>(EffectClass->GetDefaultObject());
	if (DamageNumberEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Effect.DamageNumber must use AS1DamageNumberEffect."));
		return;
	}

	DamageNumberEffect->PlayDamageNumber(GetWorld(), Damage, HitLocation);
}
