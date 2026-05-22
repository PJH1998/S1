// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"

US1GameplayAbility_Attack::US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UsedTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UsedTag);
	}

	if (bRotateToCamera)
	{
		StartRotateToCamera();
	}
}

const FS1MontageData* US1GameplayAbility_Attack::GetMontageData() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(MontageTag);
}

const FS1MontageSet* US1GameplayAbility_Attack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag); // Index default = 0
}

void US1GameplayAbility_Attack::StartRotateToCamera()
{
	if (IsValid(RotateTask))
	{
		// 태스크 재생성 없이 목표 Yaw만 갱신
		if (const APlayerController* PC = GetCurrentActorInfo()->PlayerController.Get())
		{
			RotateTask->UpdateTargetYaw(PC->GetControlRotation().Yaw);
		}
		return;
	}

	RotateTask = US1AbilityTask_RotateToCamera::RotateToCamera(this, RotationSpeed);
	RotateTask->ReadyForActivation();
}
