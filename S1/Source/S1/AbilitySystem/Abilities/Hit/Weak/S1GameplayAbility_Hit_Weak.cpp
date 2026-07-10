// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/Weak/S1GameplayAbility_Hit_Weak.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Tags/S1GameplayTags.h"

US1GameplayAbility_Hit_Weak::US1GameplayAbility_Hit_Weak(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(S1AbilityTags::Ability_Player_Hit_Weak);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = S1HitType::HitType_Weak;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(S1StateTags::State_Hit_React);
}

void US1GameplayAbility_Hit_Weak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == IsValid(MontageProgression))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartMontageProgression();
}
