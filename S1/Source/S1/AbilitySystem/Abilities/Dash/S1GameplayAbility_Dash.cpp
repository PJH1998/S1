// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Dash/S1GameplayAbility_Dash.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Progression/S1MontageProgression_Sequence.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1GameplayAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 방향 캡처 + CMC 회전 비활성 (Super 전 — 몽타주 시작 전에 설정해야 함)
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character))
	{
		CapturedDashDirection = ComputeInputDirection();

		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		bCachedOrientToMovement = CMC->bOrientRotationToMovement;
		CMC->bOrientRotationToMovement = false;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(MontageProgression))
	{
		// 입력 방향에 맞는 섹션부터 재생
		if (US1MontageProgression_Sequence* SeqProg = Cast<US1MontageProgression_Sequence>(MontageProgression))
		{
			SeqProg->SetStartSection(ComputeDirectionalSection());
		}
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}

	if (MoveBeginEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* BeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveBeginEventTag);
		BeginTask->EventReceived.AddDynamic(this, &ThisClass::OnMoveBeginReceived);
		BeginTask->ReadyForActivation();
	}

	if (MoveEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* EndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveEndEventTag);
		EndTask->EventReceived.AddDynamic(this, &ThisClass::OnMoveEndReceived);
		EndTask->ReadyForActivation();
	}
}

void US1GameplayAbility_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 안전망 — MoveEnd 이벤트 없이 종료된 경우 대비
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		CMC->GravityScale = 1.f;
		CMC->bOrientRotationToMovement = bCachedOrientToMovement;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_Dash::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (AttackAbilityGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(AttackAbilityGroupTags))
	{
		return false;
	}

	// 대쉬 중 공격 입력 — 윈도우 조건 없이 즉시 전환
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	return true;
}

void US1GameplayAbility_Dash::OnMoveBeginReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		return;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		CapturedDashDirection,
		DashSpeed,
		9999.f,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		false
	);
	MoveTask->ReadyForActivation();

	if (bControlGravity)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->GetCharacterMovement()->GravityScale = 0.f;
		}
	}
}

void US1GameplayAbility_Dash::OnMoveEndReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (bControlGravity)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
			// Air 대쉬 종료 시 플레이어 이동속도 기반 관성 유지
			// LaunchCharacter: PendingLaunchVelocity로 등록 → 다음 물리 틱에 확실히 적용
			// bXYOverride=true: 이전 Velocity 무관하게 XY 대체 / bZOverride=false: Z(중력 낙하) 유지
			Character->LaunchCharacter(CapturedDashDirection * CMC->MaxWalkSpeed, true, false);
			CMC->GravityScale = 1.f;
		}
	}
}

FName US1GameplayAbility_Dash::ComputeDirectionalSection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return SectionForward;
	}

	const FVector InputDir = Character->GetLastMovementInputVector();
	if (InputDir.IsNearlyZero())
	{
		return SectionForward;
	}

	const FVector Forward = Character->GetActorForwardVector();
	const FVector Right   = Character->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(InputDir, Forward);
	const float RightDot   = FVector::DotProduct(InputDir, Right);

	// Atan2: 오른쪽 양수, 왼쪽 음수
	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));

	if (Angle >= -45.f && Angle <= 45.f)  { return SectionForward; }
	if (Angle > 45.f  && Angle <= 135.f)  { return SectionRight; }
	if (Angle < -45.f && Angle >= -135.f) { return SectionLeft; }
	return SectionBack;
}

FVector US1GameplayAbility_Dash::ComputeInputDirection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return FVector::ForwardVector;
	}

	FVector InputDir = Character->GetLastMovementInputVector();
	if (InputDir.IsNearlyZero())
	{
		InputDir = Character->GetActorForwardVector();
	}

	InputDir.Z = 0.f;
	return InputDir.GetSafeNormal();
}
