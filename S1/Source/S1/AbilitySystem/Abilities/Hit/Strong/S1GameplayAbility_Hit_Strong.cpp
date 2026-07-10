// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/Strong/S1GameplayAbility_Hit_Strong.h"
#include "AbilitySystem/Progression/Loop/Directional/S1MontageProgression_DirectionalLoop.h"
#include "Character/S1Character.h"
#include "Tags/S1GameplayTags.h"

US1GameplayAbility_Hit_Strong::US1GameplayAbility_Hit_Strong(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(S1AbilityTags::Ability_Player_Hit_Strong);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = S1HitType::HitType_Strong;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(S1StateTags::State_Hit_React);

	// 항상 DirectionalLoop 고정 — BP에서 안 채워도(까먹어도) 항상 유효
	MontageProgression = CreateDefaultSubobject<US1MontageProgression_DirectionalLoop>(TEXT("MontageProgression"));
}

void US1GameplayAbility_Hit_Strong::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AS1Character* Character = GetValidCharacterOrEnd(Handle, ActorInfo, ActivationInfo);
	if (nullptr == Character)
	{
		return;
	}

	bool bHitFromBack = false;
	FVector HitDirection = FVector::ForwardVector;
	FVector LaunchVelocity = FVector::ZeroVector;
	ComputeKnockback(LaunchSpeed, bHitFromBack, HitDirection, LaunchVelocity);
	LaunchVelocity.Z = LaunchUpSpeed;

	// 앞이면 공격원을 정면으로, 뒤면 공격원을 등지도록(뒤에 두고) 스냅 — Strong 전용 회전 규칙
	FVector FacingDirection = bHitFromBack ? -HitDirection : HitDirection;

	// 몽타주 재생(Multicast) 직전에 명시적으로 멀티캐스트 — 같은 액터의 Reliable Multicast는 순서 보장되어
	// HitLaunch 노티파이가 발화하는 시점엔 항상 값이 세팅돼있음이 보장됨 (일반 Replicated 프로퍼티는 레이스 발생 확인됨)
	Character->MulticastSetHitLaunchData(LaunchVelocity, FacingDirection);

	if (US1MontageProgression_DirectionalLoop* DirLoopProg = Cast<US1MontageProgression_DirectionalLoop>(MontageProgression))
	{
		DirLoopProg->SetDirection(bHitFromBack ? ES1Direction::Back : ES1Direction::Forward);
	}

	StartMontageProgression();
}
