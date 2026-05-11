// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/S1GameplayAbility_Attack.h"
#include "Character/S1Player.h"

US1GameplayAbility_Attack::US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Do Something
	if (AttackMontage)
	{
		AS1Player* Player = Cast<AS1Player>(ActorInfo->AvatarActor);
		Player->PlayAnimMontage(AttackMontage);
	}

	//TEST
	ApplyCost(Handle, ActorInfo, ActivationInfo);
}
