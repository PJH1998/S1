// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"

bool US1PlayerAbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return false;
	}

	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || false == Spec->IsActive())
		{
			continue;
		}

		US1GameplayAbility* GA = Cast<US1GameplayAbility>(Spec->GetPrimaryInstance());
		if (GA && false == GA->OnInputReactivated())
		{
			// 입력 미소비 (콤보 윈도우 미오픈 등) → GA의 FlushTag로 버퍼에 저장
			QueueAbility(AbilityTag, GA->GetInputFlushTag());
			return false;
		}

		ClearQueue();
		return true;
	}

	// 대상 GA의 AbilityTags 수집 (크로스 콤보 비교용)
	FGameplayTagContainer TargetAbilityTags;
	for (auto& Handle : *Handles)
	{
		if (FGameplayAbilitySpec* TargetSpec = FindAbilitySpecFromHandle(Handle))
		{
			if (TargetSpec->Ability)
			{
				TargetAbilityTags.AppendTags(TargetSpec->Ability->AbilityTags);
			}
		}
	}

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (false == Spec.IsActive())
		{
			continue;
		}

		if (US1GameplayAbility* GA = Cast<US1GameplayAbility>(Spec.GetPrimaryInstance()))
		{
			if (GA->OnCrossInput(TargetAbilityTags))
			{
				bool bCrossActivated = false;
				for (auto& TargetHandle : *Handles)
				{
					bCrossActivated |= TryActivateAbility(TargetHandle);
				}

				if (bCrossActivated)
				{
					ClearQueue();
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				QueueAbility(AbilityTag, GA->GetInputFlushTag());
			}

			return false;
		}
	}

	bool bActivated = false;
	for (auto& Handle : *Handles)
	{
		bActivated |= TryActivateAbility(Handle);
	}

	if (bActivated)
	{
		ClearQueue();
	}
	else
	{
		// 발동 실패 — FlushTag 없이 타이머만으로 큐 관리
		QueueAbility(AbilityTag, FGameplayTag());
	}

	return bActivated;
}

void US1PlayerAbilitySystemComponent::QueueAbility(const FGameplayTag& AbilityTag, const FGameplayTag& FlushTag)
{
	ClearQueue();

	QueuedAbilityTag = AbilityTag;
	QueuedFlushTag = FlushTag;

	if (QueuedFlushTag.IsValid())
	{
		TagEventHandle = RegisterGameplayTagEvent(QueuedFlushTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ThisClass::OnFlushTagChanged);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueueClearTimer, this, &ThisClass::ClearQueue, InputBufferDuration, false);
	}
}

void US1PlayerAbilitySystemComponent::FlushQueuedAbility()
{
	FGameplayTag TagToActivate = QueuedAbilityTag;
	ClearQueue();
	ActivateAbility(TagToActivate);
}

void US1PlayerAbilitySystemComponent::ClearQueue()
{
	if (TagEventHandle.IsValid() && QueuedFlushTag.IsValid())
	{
		UnregisterGameplayTagEvent(TagEventHandle, QueuedFlushTag, EGameplayTagEventType::NewOrRemoved);
		TagEventHandle.Reset();
	}

	QueuedAbilityTag = FGameplayTag();
	QueuedFlushTag = FGameplayTag();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueueClearTimer);
	}
}

void US1PlayerAbilitySystemComponent::OnFlushTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FlushQueuedAbility();
	}
}
