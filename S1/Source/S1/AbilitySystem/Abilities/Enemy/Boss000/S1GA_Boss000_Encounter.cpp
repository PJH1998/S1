// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GA_Boss000_Encounter.h"

#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"

US1GA_Boss000_Encounter::US1GA_Boss000_Encounter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_Encounter::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageData* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(ActorInfo->AvatarActor);
	if (nullptr == Monster)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (nullptr == AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Animation
	if (PlayAttackMontage(MontageSet->Montage) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void US1GA_Boss000_Encounter::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}