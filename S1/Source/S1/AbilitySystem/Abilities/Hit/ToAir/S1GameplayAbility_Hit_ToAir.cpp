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

	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character) || false == IsValid(MontageProgression))
	{
		LOG(TEXT("[GA_Hit_ToAir] 조기 종료 — Character valid: %d, MontageProgression set: %d"),
			IsValid(Character), IsValid(MontageProgression));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bool bHitFromBack = false;
	FVector FacingDirection = FVector::ForwardVector;
	FVector LaunchVelocity = FVector::ZeroVector;
	ComputeKnockback(LaunchSpeed, bHitFromBack, FacingDirection, LaunchVelocity);
	LaunchVelocity.Z = LaunchUpSpeed;

	// 몽타주 재생(Multicast) 직전에 명시적으로 멀티캐스트 — 같은 액터의 Reliable Multicast는 순서 보장되어
	// HitLaunch 노티파이가 발화하는 시점엔 항상 값이 세팅돼있음이 보장됨 (일반 Replicated 프로퍼티는 레이스 발생 확인됨)
	Character->MulticastSetHitLaunchData(LaunchVelocity, FacingDirection);

	MontageProgression->Init(this);
	MontageProgression->OnActivated();
}
