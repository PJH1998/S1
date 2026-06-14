// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"
#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/S1AnimInstance.h"
#include "Character/Player/S1Player.h"

US1GameplayAbility_Action::US1GameplayAbility_Action(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActionStateTag    = S1StateTags::State_Action;
	MoveBeginEventTag = S1EventTags::Event_MovementStart;
	MoveEndEventTag   = S1EventTags::Event_MovementEnd;
	EarlyMoveEventTag = S1EventTags::Event_EarlyMove;
	JumpEventTag      = S1EventTags::Event_Jump;
}

void US1GameplayAbility_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 액션 중 상태 태그 부여 — Jump() 오버라이드에서 차단 조건으로 사용
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(ActionStateTag);
		}
	}

	// EarlyMove 이벤트 대기 등록
	if (EarlyMoveEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EarlyMoveEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnEarlyMoveEventReceived);
		Task->ReadyForActivation();
	}

	// Move 이벤트 바인딩 — 서브클래스 virtual dispatch를 위해 non-virtual 래퍼 사용
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (MoveBeginEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveBeginEventTag)
				.AddUObject(this, &ThisClass::InternalMoveBeginCallback);
		}
		if (MoveEndEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveEndEventTag)
				.AddUObject(this, &ThisClass::InternalMoveEndCallback);
		}
	}
}

void US1GameplayAbility_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(ActionStateTag))
			{
				ASC->RemoveLooseGameplayTag(ActionStateTag);
			}
		}
	}

	if (IsValid(MontageProgression))
	{
		MontageProgression->OnDeactivated();
	}

	// 안전망 — MoveEnd 이벤트 없이 종료된 경우 대비
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	// Move 이벤트 바인딩 해제
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (MoveBeginEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveBeginEventTag).RemoveAll(this);
		}
		if (MoveEndEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveEndEventTag).RemoveAll(this);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_Action::OnInputReactivated()
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->OnInputReactivated();
	}
	return false;
}

void US1GameplayAbility_Action::OnInputReleased()
{
	if (IsValid(MontageProgression))
	{
		MontageProgression->OnInputReleased();
	}
}

bool US1GameplayAbility_Action::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->OnCrossInput(TargetAbilityTags);
	}
	return false;
}

FGameplayTag US1GameplayAbility_Action::GetInputFlushTag() const
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->GetInputFlushTag();
	}
	return FGameplayTag();
}

void US1GameplayAbility_Action::RequestEndAbility(bool bWasCancelled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void US1GameplayAbility_Action::RequestReactivateSelf()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC))
	{
		return;
	}

	FGameplayTagContainer OwnTags;
	if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		OwnTags = Spec->Ability->AbilityTags;
	}

	if (false == OwnTags.IsValid())
	{
		return;
	}

	// EndAbility 전에 재발동 가능 여부 확인 — Blocked/Required Tag 불충족 시 현재 GA 유지
	// (ex. 공중 콤보 UsedTag — 확인 없이 끝내면 몽타주 진행 중 GravityScale 등이 복구됨)
	TArray<FGameplayAbilitySpec*> ActivatableSpecs;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(OwnTags, ActivatableSpecs);
	if (ActivatableSpecs.IsEmpty())
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	ASC->TryActivateAbilitiesByTag(OwnTags);
}

void US1GameplayAbility_Action::RequestActivateAbilityByTag(const FGameplayTagContainer& Tags)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->TryActivateAbilitiesByTag(Tags);
	}
}

US1AnimInstance* US1GameplayAbility_Action::GetAnimInstanceForProgression() const
{
	return GetAnimInstance();
}

void US1GameplayAbility_Action::OnMoveBeginReceived(const FGameplayEventData* Payload)
{
	// 이동 속도는 NotifyState에서 Payload.EventMagnitude로 전달 (음수 = 후방 이동)
	const float Impulse = Payload ? Payload->EventMagnitude : 0.f;
	if (FMath::IsNearlyZero(Impulse))
	{
		return;
	}

	if (IsValid(MoveTask))
	{
		return;
	}

	const AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	const FVector FacingDir = Player->GetActorForwardVector();
	const FVector InputDir  = Player->GetLastInputDirection();

	// S키 (반대 방향) 누르고 있으면 이동 없음
	if (InputDir.SizeSquared() > 0.1f && FVector::DotProduct(FacingDir, InputDir) < -0.5f)
	{
		return;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		FacingDir,
		Impulse,
		9999.f,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		true	// bEnableGravity — XY만 루트모션이 덮고 Z는 물리(런치/중력)에 위임
	);
	MoveTask->ReadyForActivation();
}

void US1GameplayAbility_Action::OnMoveEndReceived(const FGameplayEventData* Payload)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}
}

void US1GameplayAbility_Action::InternalMoveBeginCallback(const FGameplayEventData* Payload)
{
	OnMoveBeginReceived(Payload);
}

void US1GameplayAbility_Action::InternalMoveEndCallback(const FGameplayEventData* Payload)
{
	OnMoveEndReceived(Payload);
}

void US1GameplayAbility_Action::OnEarlyMoveEventReceived(FGameplayEventData Payload)
{
	OnEarlyMoveEnabled();
}

void US1GameplayAbility_Action::OnEarlyMoveEnabled()
{
	// 점프 차단 해제
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(ActionStateTag);
		}
	}

	// 이동/점프 입력 감지 시작 → TriggerInputDetected
	US1AbilityTask_EarlyExitChecker* Task = US1AbilityTask_EarlyExitChecker::Run(this, JumpEventTag);
	Task->OnInputDetected.AddUObject(this, &ThisClass::OnEarlyMoveTriggered);
	Task->ReadyForActivation();
}

void US1GameplayAbility_Action::OnEarlyMoveTriggered()
{
	if (CurrentActorInfo)
	{
		if (UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance())
		{
			AnimInst->Montage_Stop(0.15f);
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
