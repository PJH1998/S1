// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystemComponent.h"
#include "Character/S1Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1LogChannels.h"

US1GameplayAbility_Hit::US1GameplayAbility_Hit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 피격 판정은 서버만 앎(피격자가 예측할 근거가 없음) → 항상 서버 권위
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// 이동/점프로 조기 탈출 불가 — Action 베이스의 EarlyMove 비활성화
	EarlyMoveEventTag = FGameplayTag();
}

void US1GameplayAbility_Hit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AttackerActor = (nullptr != TriggerEventData) ? TriggerEventData->Instigator : nullptr;

	LOG(TEXT("[GA_Hit] %s ActivateAbility — Avatar: %s, Attacker: %s"),
		*GetClass()->GetName(),
		GetAvatarActorFromActorInfo() ? *GetAvatarActorFromActorInfo()->GetName() : TEXT("None"),
		AttackerActor.IsValid() ? *AttackerActor->GetName() : TEXT("None"));

	// 피격 시 진행중이던 액션 전부 중지 — 쿨다운은 GE라 CancelAllAbilities로는 안 끊김
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->CancelAllAbilities(this);
	}

	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		// 스프린트 등 잔존 속도/가속도 제거 — State.Action은 새 입력만 막고 기존 Velocity는 안 지워서
		// 정지형 피격(Weak)이 관성으로 미끄러지며 반응이 무시된 것처럼 보이는 문제 방지
		if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
		{
			CMC->StopMovementImmediately();
		}

		// Assault 등 SpawnPersistentEffect로 띄운 이펙트가 몽타주 인터럽트로 DespawnEffect 노티파이를
		// 못 받아 영구 잔류하는 경우의 안전망 — 피격 시 보유 중인 이펙트 전부 강제 정리
		Character->MulticastClearAllAttachedEffects();
	}
}

void US1GameplayAbility_Hit::ComputeKnockback(float LaunchSpeed, bool& bOutHitFromBack, FVector& OutHitDirection, FVector& OutLaunchVelocity) const
{
	const AActor* Character = GetAvatarActorFromActorInfo();

	FVector HitDir = IsValid(Character) ? Character->GetActorForwardVector() : FVector::ForwardVector;
	if (AttackerActor.IsValid() && IsValid(Character))
	{
		FVector ToAttacker = AttackerActor->GetActorLocation() - Character->GetActorLocation();
		ToAttacker.Z = 0.f;
		if (false == ToAttacker.IsNearlyZero())
		{
			HitDir = ToAttacker.GetSafeNormal();
		}
	}

	bOutHitFromBack = IsValid(Character) && FVector::DotProduct(Character->GetActorForwardVector(), HitDir) < 0.f;

	// 공격원을 향하는 방향(정면 스냅 여부는 호출부가 bOutHitFromBack으로 직접 판단)
	OutHitDirection = HitDir;

	// 회전 분기와 무관하게 항상 공격원 반대 방향(월드 기준)
	OutLaunchVelocity = -HitDir * LaunchSpeed;
}

AS1Character* US1GameplayAbility_Hit::GetValidCharacterOrEnd(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character) || false == IsValid(MontageProgression))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return nullptr;
	}
	return Character;
}

void US1GameplayAbility_Hit::StartMontageProgression()
{
	MontageProgression->Init(this);
	MontageProgression->OnActivated();
}
