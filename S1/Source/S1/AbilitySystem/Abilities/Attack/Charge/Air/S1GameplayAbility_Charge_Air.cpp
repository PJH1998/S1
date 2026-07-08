// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Charge/Air/S1GameplayAbility_Charge_Air.h"
#include "Character/S1Character.h"
#include "NiagaraComponent.h"

void US1GameplayAbility_Charge_Air::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 마지막 사이클까지 다 돌았으면 OnProgressionCycleCompleted에서 이미 정리됐음 — 여기선 인터럽트 등 안전망
	if (ActiveEffectTag.IsValid())
	{
		if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
		{
			// 인터럽트로 중간에 끝난 경우도 시스템이 진짜 완료되는 시점에 맞춰 정리(안 걸면 영원히 안 사라짐)
			const TArray<TWeakObjectPtr<UNiagaraComponent>>* Effects = Character->FindAttachedEffects(ActiveEffectTag);
			if (nullptr != Effects && Effects->Num() > 0)
			{
				Character->ArmEffectAutoDestroy(Effects->Last().Get());
			}

			Character->EndAttachedEffect(ActiveEffectTag);
		}
		ActiveEffectTag = FGameplayTag();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Charge_Air::OnProgressionCycleCompleted(FGameplayTag EffectTag, int32 CycleCount, int32 MaxCycleCount)
{
	if (false == EffectTag.IsValid())
	{
		return;
	}

	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return;
	}

	const TArray<TWeakObjectPtr<UNiagaraComponent>>* Effects = Character->FindAttachedEffects(EffectTag);
	UNiagaraComponent* Component = (nullptr != Effects && Effects->Num() > 0) ? Effects->Last().Get() : nullptr;
	if (false == IsValid(Component))
	{
		return;
	}

	ActiveEffectTag = EffectTag;

	const float Ratio = (MaxCycleCount > 0)
		? FMath::Clamp(static_cast<float>(CycleCount) / static_cast<float>(MaxCycleCount), 0.f, 1.f)
		: 1.f;

	// 이번 사이클 값을 먼저 반영한 다음 재생 트리거 — Activate가 먼저면 이전 사이클 값으로 버스트가 나갈 수 있음
	Component->SetVariableFloat(ChargeRatioParameterName, Ratio);
	Component->SetRelativeScale3D(FVector::OneVector * FMath::Lerp(MinScale, MaxScale, Ratio));

	// 같은 컴포넌트를 처음부터 재생(버스트 재트리거)
	Component->Activate(true);

	// 마지막 사이클(최대 충전) 도달 시 바로 다음(End) 몽타주로 넘어가므로 여기서 바로 정리
	if (MaxCycleCount > 0 && CycleCount >= MaxCycleCount)
	{
		// 파티클까지 완전히 다 죽어 시스템이 진짜 끝나는 시점에 스스로 파괴되도록 지금(마지막 사이클) 예약
		// — 스폰 시점에 걸면 매 재트리거(Activate)마다 발화해서 첫 사이클만에 파괴돼버림
		Character->ArmEffectAutoDestroy(Component);

		// LifetimeAfterEnd를 0(즉시)이 아니라 살짝 줘야 함 — 0이면 방금 위에서 건 Activate(true)가 같은 프레임에
		// EndAttachedEffect의 즉시 Deactivate로 취소되어 마지막 버스트가 아예 안 뜸
		Character->EndAttachedEffect(EffectTag, 0.1f);
		ActiveEffectTag = FGameplayTag();
	}
}
