// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Death/S1GameplayAbility_Death.h"

#include "Character/Player/S1Player.h"
#include "Tags/S1GameplayTags.h"

US1GameplayAbility_Death::US1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Hit_Weak 생성자가 채워둔 Hit 트리거/태그를 Death 전용으로 덮어씀
	AbilityTags.Reset();
	AbilityTags.AddTag(S1AbilityTags::Ability_Player_Death);

	AbilityTriggers.Reset();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag    = S1HitType::HitType_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(S1StateTags::State_Dead);
}

void US1GameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == IsActive())
	{
		return;
	}

	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		Player->DisableCollisionForDeath();
	}
}
