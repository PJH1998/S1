// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/ToAir/S1GameplayAbility_Hit_ToAir.h"
#include "Character/S1Character.h"
#include "Tags/S1GameplayTags.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "S1LogChannels.h"

US1GameplayAbility_Hit_ToAir::US1GameplayAbility_Hit_ToAir(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(S1AbilityTags::Ability_Player_Hit_ToAir);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = S1HitType::HitType_ToAir;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(S1StateTags::State_Hit_Launch);
}

void US1GameplayAbility_Hit_ToAir::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AS1Character* Character = GetValidCharacterOrEnd(Handle, ActorInfo, ActivationInfo);
	if (nullptr == Character)
	{
		LOG(TEXT("[GA_Hit_ToAir] 조기 종료 — Character/MontageProgression invalid"));
		return;
	}

	bool bHitFromBack = false;
	FVector HitDirection = FVector::ForwardVector;
	FVector LaunchVelocity = FVector::ZeroVector;
	ComputeKnockback(LaunchSpeed, bHitFromBack, HitDirection, LaunchVelocity);
	LaunchVelocity.Z = LaunchUpSpeed;

	// ToAir는 피격 방향과 무관하게 항상 공격원을 정면으로 바라봄 (Strong과 달리 뒤 피격 스냅 없음)
	// 몽타주 재생(Multicast) 직전에 명시적으로 멀티캐스트 — 같은 액터의 Reliable Multicast는 순서 보장되어
	// HitLaunch 노티파이가 발화하는 시점엔 항상 값이 세팅돼있음이 보장됨 (일반 Replicated 프로퍼티는 레이스 발생 확인됨)
	Character->MulticastSetHitLaunchData(LaunchVelocity, HitDirection);

	StartMontageProgression();
}
