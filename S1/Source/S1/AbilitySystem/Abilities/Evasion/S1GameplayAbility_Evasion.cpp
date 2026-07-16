// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Evasion/S1GameplayAbility_Evasion.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Animation/NotifyState/LooseGameplayTag/S1AnimNotifyState_LooseGameplayTag.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"
#include "Tags/S1GameplayTags.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Character/S1Character.h"
#include "GameFramework/CharacterMovementComponent.h"

US1GameplayAbility_Evasion::US1GameplayAbility_Evasion()
{
	// 이동 루트모션 예측 — AbilityTask_ApplyRootMotion이 소유 클라에서 예측 실행
	// ⚠️ §39: BP에 저장된 값이 C++ 기본값을 이김 → GA_Dash/GA_Dodge BP Class Defaults에서도 Local Predicted 설정 필요
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 자체 중력 제어 — 이동 루트모션이 Z까지 Override(=구간 중 부유) → 공중 회피 float를 LaunchCharacter 없이 처리
	bMoveEnableGravity = false;
}

void US1GameplayAbility_Evasion::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Super 전 — 몽타주 시작 전 방향 캡처 + 회전 제어권 확보
	CapturedMoveDirection = ComputeInputDirection();

	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		Character->PushOrientRotationOverride();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// Move 이벤트 바인딩은 GA_Action::ActivateAbility에서 처리
}

void US1GameplayAbility_Evasion::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 안전망 — MoveEnd 이벤트 없이 종료된 경우 대비
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	// 무적 윈도우 도중 종료 시 — 타이머 정리 + 서버 태그 강제 제거
	ClearInvincibilityTimers();

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (IsValid(Avatar) && Avatar->HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(S1StateTags::State_Invincible))
			{
				ASC->RemoveLooseGameplayTag(S1StateTags::State_Invincible);
			}
		}
	}

	// 복제 경유로 중력 복구
	ResetGravityScale();

	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		Character->PopOrientRotationOverride();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FVector US1GameplayAbility_Evasion::GetCapturedMoveDirection() const
{
	if (false == CapturedMoveDirection.IsNearlyZero())
	{
		return CapturedMoveDirection;
	}
	return Super::GetCapturedMoveDirection();
}

FVector US1GameplayAbility_Evasion::ComputeInputDirection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return FVector::ForwardVector;
	}

	// 입력 방향은 CMC Acceleration에서 — GetLastMovementInputVector()는 로컬 입력 개념이라
	// 서버의 autonomous proxy에서 항상 0 → 서버가 정면(ActorForward)으로 폴백 → 클라(입력방향) 예측과
	// 불일치하여 서버 권위 보정으로 정면 스냅. Acceleration은 ServerMove로 서버에 복제되어 클라/서버 일치
	FVector InputDir = FVector::ZeroVector;
	if (const UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
	{
		InputDir = CMC->GetCurrentAcceleration();
	}

	InputDir.Z = 0.f;
	if (InputDir.IsNearlyZero())
	{
		return Character->GetActorForwardVector();
	}
	return InputDir.GetSafeNormal();
}

void US1GameplayAbility_Evasion::OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate)
{
	ClearInvincibilityTimers();
	ScheduleInvincibilityWindows(Montage, Rate);
}

void US1GameplayAbility_Evasion::ScheduleInvincibilityWindows(UAnimMontage* Montage, float Rate)
{
	// 데미지 블록(PreAttributeChange)은 서버에서만 판정 → 무적 태그도 서버에서만 관리
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (false == IsValid(Avatar) || false == Avatar->HasAuthority())
	{
		return;
	}

	UWorld* World = Avatar->GetWorld();
	if (nullptr == Montage || nullptr == World)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC))
	{
		return;
	}

	const float EffectiveRate = FMath::Max(Rate * Montage->RateScale, KINDA_SMALL_NUMBER);

	for (const FAnimNotifyEvent& Event : Montage->Notifies)
	{
		const US1AnimNotifyState_LooseGameplayTag* TagNotify = Cast<US1AnimNotifyState_LooseGameplayTag>(Event.NotifyStateClass);
		if (nullptr == TagNotify || TagNotify->GetTag() != S1StateTags::State_Invincible)
		{
			continue;
		}

		const float RealBegin = Event.GetTriggerTime() / EffectiveRate;
		const float RealEnd   = (Event.GetTriggerTime() + Event.GetDuration()) / EffectiveRate;

		FTimerHandle BeginHandle;
		World->GetTimerManager().SetTimer(
			BeginHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, ASC]()
			{
				ASC->AddLooseGameplayTag(S1StateTags::State_Invincible);
			}),
			FMath::Max(RealBegin, 0.001f), false);
		InvincibilityTimers.Add(BeginHandle);

		FTimerHandle EndHandle;
		World->GetTimerManager().SetTimer(
			EndHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, ASC]()
			{
				ASC->RemoveLooseGameplayTag(S1StateTags::State_Invincible);
			}),
			FMath::Max(RealEnd, 0.002f), false);
		InvincibilityTimers.Add(EndHandle);
	}
}

void US1GameplayAbility_Evasion::ClearInvincibilityTimers()
{
	if (UWorld* World = GetWorld())
	{
		for (FTimerHandle& Handle : InvincibilityTimers)
		{
			World->GetTimerManager().ClearTimer(Handle);
		}
	}
	InvincibilityTimers.Reset();
}

