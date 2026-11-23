// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Effects/S1GameplayEffect_Heal.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "Tags/S1GameplayTags.h"

US1GameplayEffect_Heal::US1GameplayEffect_Heal()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat HealAmount;
	HealAmount.DataTag = S1SetByCallerTags::SetByCaller_Heal;

	FGameplayModifierInfo HealModifier;
	HealModifier.Attribute         = US1AttributeSet::GetHealthAttribute();
	HealModifier.ModifierOp        = EGameplayModOp::Additive;
	HealModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealAmount);

	Modifiers.Add(HealModifier);
}
