// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GA_Boss000_Attack00.h"
#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"

US1GA_Boss000_Attack00::US1GA_Boss000_Attack00(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GA_Boss000_Attack00::OnInputReactivated()
{
	Super::OnInputReactivated();
}

void US1GA_Boss000_Attack00::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageSet* MontageSet = GetMontage();
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
	Monster->PlayAnimation(ActiveMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &US1GA_Boss000_Attack00::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ActiveMontage);
}

void US1GA_Boss000_Attack00::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ActiveMontage = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GA_Boss000_Attack00::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

const FS1MontageSet* US1GA_Boss000_Attack00::GetMontage() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag, 0);
}
