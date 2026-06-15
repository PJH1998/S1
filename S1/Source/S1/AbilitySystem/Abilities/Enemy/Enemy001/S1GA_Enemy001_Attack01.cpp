// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/Enemy001/S1GA_Enemy001_Attack01.h"

#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "Tags/S1GameplayTags.h"

US1GA_Enemy001_Attack01::US1GA_Enemy001_Attack01(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AnimDataTag = S1AssetTags::Asset_AnimData;
	MontageTag = S1AnimTags::Anim_Montage_Enemy001_Attack01;
	CooldownTags.AddTag(S1CooldownTags::Cooldown_Enemy001_Attack01);
}

void US1GA_Enemy001_Attack01::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == IsActive())
	{
		return;
	}

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

	ActiveMontage = MontageSet->Montage;

	const float Duration = Monster->PlayAnimMontage(ActiveMontage, 1.f, StartSection);
	if (Duration <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AnimInstance->Montage_SetNextSection(StartSection, LoopSection, ActiveMontage);
	AnimInstance->Montage_SetNextSection(LoopSection, EndSection, ActiveMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ActiveMontage);
}
