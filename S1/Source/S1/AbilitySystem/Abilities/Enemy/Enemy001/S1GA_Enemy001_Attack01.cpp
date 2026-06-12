// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/Enemy001/S1GA_Enemy001_Attack01.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AI/S1AIController.h"
#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "S1GameplayTags.h"

US1GA_Enemy001_Attack01::US1GA_Enemy001_Attack01(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AnimDataTag = S1AssetTags::Asset_AnimData;
	MontageTag = S1AnimTags::Anim_Montage_Enemy001_Attack01;
	CheckDistanceEventTag = S1EventTags::Event_CheckDistance;
	CooldownTags.AddTag(S1CooldownTags::Cooldown_Enemy001_Attack01);
}

void US1GA_Enemy001_Attack01::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == IsActive())
	{
		return;
	}

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

	if (CheckDistanceEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CheckDistanceEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnCheckDistanceEvent);
		Task->ReadyForActivation();
	}
}

void US1GA_Enemy001_Attack01::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GA_Enemy001_Attack01::OnCheckDistanceEvent(FGameplayEventData Payload)
{
	UpdateNextSectionByDistance();
}

void US1GA_Enemy001_Attack01::UpdateNextSectionByDistance()
{
	AS1Monster* Monster = Cast<AS1Monster>(GetAvatarActorFromActorInfo());
	UAnimInstance* AnimInstance = Monster ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == AnimInstance || nullptr == ActiveMontage)
	{
		return;
	}

	const FName NextSection = GetDistanceToTarget() > LoopDistance ? LoopSection : EndSection;

	AnimInstance->Montage_SetNextSection(StartSection, NextSection, ActiveMontage);
	AnimInstance->Montage_SetNextSection(LoopSection, EndSection, ActiveMontage);
}

float US1GA_Enemy001_Attack01::GetDistanceToTarget() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (nullptr == AvatarActor)
	{
		return 0.f;
	}

	const AActor* TargetActor = GetTargetActor();
	if (nullptr == TargetActor)
	{
		return 0.f;
	}

	return FVector::Dist2D(AvatarActor->GetActorLocation(), TargetActor->GetActorLocation());
}

AActor* US1GA_Enemy001_Attack01::GetTargetActor() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const AS1AIController* AIController = OwnerPawn ? Cast<AS1AIController>(OwnerPawn->GetController()) : nullptr;
	return AIController ? AIController->GetTargetActor() : nullptr;
}
