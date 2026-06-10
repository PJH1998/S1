// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/S1AnimInstance.h"
#include "Character/Player/S1Player.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"
#include "Weapon/S1Weapon.h"

void US1GameplayAbility_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 액션 중 상태 태그 부여 — Jump() 오버라이드에서 차단 조건으로 사용
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(ActionStateTag);
		}
	}

	// EarlyMove 이벤트 대기 등록
	if (EarlyMoveEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EarlyMoveEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnEarlyMoveEventReceived);
		Task->ReadyForActivation();
	}
}

void US1GameplayAbility_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(ActionStateTag))
			{
				ASC->RemoveLooseGameplayTag(ActionStateTag);
			}
		}
	}

	if (IsValid(MontageProgression))
	{
		MontageProgression->OnDeactivated();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_Action::OnInputReactivated()
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->OnInputReactivated();
	}
	return false;
}

bool US1GameplayAbility_Action::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->OnCrossInput(TargetAbilityTags);
	}
	return false;
}

FGameplayTag US1GameplayAbility_Action::GetInputFlushTag() const
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->GetInputFlushTag();
	}
	return FGameplayTag();
}

void US1GameplayAbility_Action::RequestEndAbility(bool bWasCancelled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void US1GameplayAbility_Action::RequestReactivateSelf()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC))
	{
		return;
	}

	FGameplayTagContainer OwnTags;
	if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		OwnTags = Spec->Ability->AbilityTags;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	if (OwnTags.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(OwnTags);
	}
}

void US1GameplayAbility_Action::RequestActivateAbilityByTag(const FGameplayTagContainer& Tags)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->TryActivateAbilitiesByTag(Tags);
	}
}

const FS1MontageData* US1GameplayAbility_Action::GetMontageData() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (false == IsValid(AnimData))
	{
		return nullptr;
	}

	FGameplayTag ResolvedTag = MontageTag;

	if (bUseWeaponMontage)
	{
		if (const AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
		{
			if (const AS1Weapon* Weapon = Player->GetEquippedWeapon())
			{
				if (const FGameplayTag* WeaponTag = MontageTagByWeapon.Find(Weapon->GetWeaponType()))
				{
					ResolvedTag = *WeaponTag;
				}
			}
		}
	}

	return AnimData->FindMontageByTag(ResolvedTag);
}

const FS1MontageData* US1GameplayAbility_Action::GetMontageDataByTag(FGameplayTag InMontageTag) const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (false == IsValid(AnimData))
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(InMontageTag);
}

const FS1MontageSet* US1GameplayAbility_Action::GetCurrentMontageSet() const
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->GetCurrentMontageSet();
	}

	const FS1MontageData* Data = GetMontageData();
	if (nullptr == Data || Data->MontageSets.IsEmpty())
	{
		return nullptr;
	}
	return &Data->MontageSets[0];
}

US1AnimInstance* US1GameplayAbility_Action::GetAnimInstanceForProgression() const
{
	return GetAnimInstance();
}

void US1GameplayAbility_Action::OnEarlyMoveEventReceived(FGameplayEventData Payload)
{
	OnEarlyMoveEnabled();
}

void US1GameplayAbility_Action::OnEarlyMoveEnabled()
{
	// 점프 차단 해제
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(ActionStateTag);
		}
	}

	// 이동/점프 입력 감지 시작 → TriggerInputDetected
	US1AbilityTask_EarlyExitChecker* Task = US1AbilityTask_EarlyExitChecker::Run(this, JumpEventTag);
	Task->OnInputDetected.AddUObject(this, &ThisClass::OnEarlyMoveTriggered);
	Task->ReadyForActivation();
}

void US1GameplayAbility_Action::OnEarlyMoveTriggered()
{
	if (CurrentActorInfo)
	{
		if (UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance())
		{
			AnimInst->Montage_Stop(0.15f);
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
