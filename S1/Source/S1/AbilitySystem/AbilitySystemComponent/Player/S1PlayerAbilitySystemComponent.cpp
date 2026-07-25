// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Character/Player/S1Player.h"
#include "Tags/S1GameplayTags.h"

US1PlayerAbilitySystemComponent::US1PlayerAbilitySystemComponent()
{
	// PlayerState ASC — 소유 클라엔 풀 동기화, 타 클라엔 GameplayCue/태그만 (멀티 표준)
	SetIsReplicated(true);
	SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CooldownVariants.Add(FS1CooldownVariantSet{
		S1AbilityTags::Ability_Player_Attack_Skill01,
		S1CooldownTags::Cooldown_Player_Ground_Skill01,
		S1CooldownTags::Cooldown_Player_Air_Skill01 });

	CooldownVariants.Add(FS1CooldownVariantSet{
		S1AbilityTags::Ability_Player_Attack_Skill02,
		S1CooldownTags::Cooldown_Player_Ground_Skill02,
		S1CooldownTags::Cooldown_Player_Air_Skill02 });

	CooldownVariants.Add(FS1CooldownVariantSet{
		S1AbilityTags::Ability_Player_Attack_Skill03,
		S1CooldownTags::Cooldown_Player_Ground_Skill03,
		S1CooldownTags::Cooldown_Player_Air_Skill03 });

	CooldownVariants.Add(FS1CooldownVariantSet{
		S1AbilityTags::Ability_Player_Attack_Skill04,
		S1CooldownTags::Cooldown_Player_Ground_Skill04,
		S1CooldownTags::Cooldown_Player_Air_Skill04 });

	// Air 변형 없음 — GetSkillCooldown이 AirCooldownTag 무효 시 Ground 태그로 폴백
	CooldownVariants.Add(FS1CooldownVariantSet{
		S1AbilityTags::Ability_Player_Action_HealItem,
		S1CooldownTags::Cooldown_Player_HealItem,
		FGameplayTag() });
}

bool US1PlayerAbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	// 이번 호출에서 재활성화 예측키 새로 캡처 (이전 값 클리어)
	PendingReactivationKey = FPredictionKey();

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
		if (GA)
		{
			// 예측 GA + 소유 클라(비권위)면 새 예측키 스코프 안에서 재활성화 → 콤보 어드밴스 루트모션이
			// 공유 키로 생성됨 (서버는 ServerReactivateAbility에서 같은 키로 미러 → 서버가 클라 이동 인정)
			const bool bPredictReactivation =
				(false == IsOwnerActorAuthoritative()) &&
				(EGameplayAbilityNetExecutionPolicy::LocalPredicted == GA->GetNetExecutionPolicy());

			bool bConsumed = false;
			if (bPredictReactivation)
			{
				// 윈도우가 ASC의 ScopedPredictionKey에 새 키를 설정 — 윈도우 생존 중에 읽어 보관
				FScopedPredictionWindow ScopedPrediction(this, true);
				bConsumed = GA->OnInputReactivated();
				if (bConsumed)
				{
					PendingReactivationKey = ScopedPredictionKey;
				}
			}
			else
			{
				bConsumed = GA->OnInputReactivated();
			}

			if (false == bConsumed)
			{
				// 입력 미소비 (콤보 윈도우 미오픈 등) → GA의 FlushTag로 버퍼에 저장
				QueueAbility(AbilityTag, GA->GetInputFlushTag());
				return false;
			}
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

void US1PlayerAbilitySystemComponent::HandleServerInputPressed(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return;
	}

	// 활성 GA에 대한 재입력(콤보 진행) — 서버 GA 인스턴스에 반영
	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || false == Spec->IsActive())
		{
			continue;
		}

		if (US1GameplayAbility* GA = Cast<US1GameplayAbility>(Spec->GetPrimaryInstance()))
		{
			GA->OnInputReactivated();
		}
		return;
	}

	// 크로스 콤보 — 다른 활성 GA가 이 입력을 받아 전환
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
				for (auto& TargetHandle : *Handles)
				{
					TryActivateAbility(TargetHandle);
				}
			}
			return;
		}
	}
}

FPredictionKey US1PlayerAbilitySystemComponent::ConsumePendingReactivationKey()
{
	const FPredictionKey Key = PendingReactivationKey;
	PendingReactivationKey = FPredictionKey();
	return Key;
}

bool US1PlayerAbilitySystemComponent::GetSkillCooldown(const FGameplayTag& UnifiedCooldownTag, float& OutRemainingTime, float& OutDuration) const
{
	OutRemainingTime = 0.f;
	OutDuration = 0.f;

	// 게이지 Max — 이 태그의 실제 발동 후보는 Ultimate(쿨타임 없음)이므로 Skill04 쿨타임 표시를 숨김
	if (UnifiedCooldownTag == S1AbilityTags::Ability_Player_Attack_Skill04 &&
		HasMatchingGameplayTag(S1StateTags::State_CanUltimate))
	{
		return false;
	}

	const FS1CooldownVariantSet* Variant = FindCooldownVariant(UnifiedCooldownTag);
	if (Variant == nullptr)
	{
		return false;
	}

	const bool bAir = HasMatchingGameplayTag(S1StateTags::State_Air) && Variant->AirCooldownTag.IsValid();
	const FGameplayTag& TagToQuery = bAir ? Variant->AirCooldownTag : Variant->GroundCooldownTag;
	if (!TagToQuery.IsValid())
	{
		return false;
	}

	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(TagToQuery));
	const TArray<TPair<float, float>> Results = GetActiveEffectsTimeRemainingAndDuration(Query);

	bool bFound = false;
	for (const TPair<float, float>& Pair : Results)
	{
		if (Pair.Value <= 0.f)
		{
			// Duration 없는(Instant/Infinite) GE는 UI 쿨다운 대상이 아님
			continue;
		}

		if (!bFound || Pair.Key > OutRemainingTime)
		{
			OutRemainingTime = FMath::Max(Pair.Key, 0.f);
			OutDuration = Pair.Value;
			bFound = true;
		}
	}

	return bFound && OutRemainingTime > 0.f;
}

const FS1CooldownVariantSet* US1PlayerAbilitySystemComponent::FindCooldownVariant(const FGameplayTag& UnifiedCooldownTag) const
{
	return CooldownVariants.FindByPredicate([&UnifiedCooldownTag](const FS1CooldownVariantSet& Variant)
	{
		return Variant.AbilityTag == UnifiedCooldownTag;
	});
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

	// 버퍼 플러시도 입력 라우팅(AS1Player)을 거쳐야 함 — ASC에서 직접 ActivateAbility 호출 시
	// 예측 재활성화의 ServerReactivateAbility RPC가 누락되어 서버가 콤보를 진행 못함 (연타 desync)
	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActor()))
	{
		Player->ActivateAbility(TagToActivate);
		return;
	}

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
