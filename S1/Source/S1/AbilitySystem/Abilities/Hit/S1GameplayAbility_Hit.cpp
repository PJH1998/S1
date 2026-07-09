// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
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
}

void US1GameplayAbility_Hit::ComputeKnockback(float LaunchSpeed, bool& bOutHitFromBack, FVector& OutFacingDirection, FVector& OutLaunchVelocity) const
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

	// 앞이면 공격원을 정면으로, 뒤면 공격원을 등지도록(뒤에 두고) 스냅
	OutFacingDirection = bOutHitFromBack ? -HitDir : HitDir;

	// 회전 분기와 무관하게 항상 공격원 반대 방향(월드 기준)
	OutLaunchVelocity = -HitDir * LaunchSpeed;
}
