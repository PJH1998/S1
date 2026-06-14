// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

US1MontageProgression_Loop::US1MontageProgression_Loop()
{
	LoopStartEventTag = S1EventTags::Event_LoopStart;
	LoopCycleEventTag = S1EventTags::Event_LoopCycle;
}

void US1MontageProgression_Loop::OnActivated()
{
	bEndRequested  = false;
	bInLoopPhase   = false;
	CurrentEndMontage = EndMontage;

	if (false == GA.IsValid())
	{
		return;
	}

	if (false == IsValid(LoopMontage))
	{
		GA->RequestEndAbility(true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	if (IsValid(StartMontage))
	{
		const float Duration = Character->PlayAnimMontage(StartMontage);
		if (Duration <= 0.f)
		{
			GA->RequestEndAbility(true);
			return;
		}

		if (LoopStartEventTag.IsValid())
		{
			// 이벤트 기반: StartMontage 마지막 프레임 AnimNotify → LoopMontage 시작
			// StartDive 등 이전 notify는 이미 발화된 상태 → 순서 보장
			UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
			if (IsValid(ASC))
			{
				ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopStartEventTag)
					.AddUObject(this, &ThisClass::OnLoopStartEventReceived);
			}
		}
		else
		{
			// 폴백: BlendingOut 기반 (StartMontage BlendOut > 0 필요)
			US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
			if (IsValid(AnimInst))
			{
				FOnMontageBlendingOutStarted BlendingOutDelegate;
				BlendingOutDelegate.BindUObject(this, &ThisClass::OnStartMontageBlendingOut);
				AnimInst->Montage_SetBlendingOutDelegate(BlendingOutDelegate, StartMontage);
			}
		}
	}
	else
	{
		PlayLoopMontage();
	}

	if (LoopEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(GA.Get(), LoopEndEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnLoopEndEventReceived);
		Task->ReadyForActivation();
	}
}

void US1MontageProgression_Loop::OnDeactivated()
{
	// LoopStartEventTag / LoopCycleEventTag는 직접 구독이므로 수동 해제 필요
	// LoopEndEventTag는 AbilityTask가 처리 — 해제 불필요
	if (GA.IsValid())
	{
		UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
		if (IsValid(ASC))
		{
			if (LoopStartEventTag.IsValid())
			{
				ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopStartEventTag).RemoveAll(this);
			}
			if (LoopCycleEventTag.IsValid())
			{
				ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopCycleEventTag).RemoveAll(this);
			}
		}
	}

	bEndRequested     = false;
	bInLoopPhase      = false;
	CurrentEndMontage = nullptr;
}

bool US1MontageProgression_Loop::OnInputReactivated()
{
	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	GA->RequestReactivateSelf();
	return true;
}

bool US1MontageProgression_Loop::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (CrossComboGroupTags.IsEmpty() || false == TargetAbilityTags.HasAny(CrossComboGroupTags))
	{
		return false;
	}

	if (false == GA.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC) || false == ASC->HasMatchingGameplayTag(CanNextAttackTag))
	{
		return false;
	}

	GA->RequestEndAbility(false);
	return true;
}

void US1MontageProgression_Loop::ExitLoop(UAnimMontage* TargetEnd, bool bImmediate)
{
	if (bEndRequested)
	{
		return;
	}

	bEndRequested     = true;
	CurrentEndMontage = IsValid(TargetEnd) ? TargetEnd : EndMontage.Get();

	if (bImmediate && bInLoopPhase)
	{
		// Loop 단계에서만 즉시 중단 — Start 재생 중엔 LoopStart Notify가 처리
		US1AnimInstance* AnimInst = GA.IsValid() ? GA->GetAnimInstanceForProgression() : nullptr;
		if (IsValid(AnimInst))
		{
			AnimInst->Montage_Stop(0.0f, LoopMontage);
		}
		PlayEndMontage();
	}
	// else: LoopStart Notify or OnLoopMontageBlendingOut 에서 처리
}

void US1MontageProgression_Loop::PlayLoopMontage()
{
	if (false == GA.IsValid())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	const float Duration = Character->PlayAnimMontage(LoopMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	bInLoopPhase = true;

	if (LoopCycleEventTag.IsValid())
	{
		// 이벤트 기반: LoopMontage 끝 직전 AnimNotify → count 증가 + Loop or End 즉시 결정
		// RemoveAll 후 AddUObject — PlayLoopMontage 재호출 시 중복 바인딩 방지
		UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo();
		if (IsValid(ASC))
		{
			auto& EventDelegate = ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopCycleEventTag);
			EventDelegate.RemoveAll(this);
			EventDelegate.AddUObject(this, &ThisClass::OnLoopCycleEventReceived);
		}
	}
	else
	{
		// 폴백: BlendingOut 기반 (LoopMontage BlendOut > 0 필요)
		US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
		if (IsValid(AnimInst))
		{
			FOnMontageBlendingOutStarted BlendingOutDelegate;
			BlendingOutDelegate.BindUObject(this, &ThisClass::OnLoopMontageBlendingOut);
			AnimInst->Montage_SetBlendingOutDelegate(BlendingOutDelegate, LoopMontage);
		}
	}
}

void US1MontageProgression_Loop::PlayEndMontage()
{
	if (false == GA.IsValid())
	{
		return;
	}

	if (false == IsValid(CurrentEndMontage))
	{
		GA->RequestEndAbility(false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GA->GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		GA->RequestEndAbility(true);
		return;
	}

	const float Duration = Character->PlayAnimMontage(CurrentEndMontage);
	if (Duration <= 0.f)
	{
		GA->RequestEndAbility(true);
		return;
	}

	US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression();
	if (IsValid(AnimInst))
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnEndMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, CurrentEndMontage);
	}
}

void US1MontageProgression_Loop::OnStartMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != StartMontage || bInterrupted)
	{
		// bInterrupted=true: ExitLoop(bImmediate) 에서 이미 처리됨
		return;
	}

	if (bEndRequested)
	{
		PlayEndMontage();
	}
	else
	{
		PlayLoopMontage();
	}
}

void US1MontageProgression_Loop::OnLoopMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != LoopMontage || bInterrupted)
	{
		// bInterrupted=true: ExitLoop(bImmediate) 에서 이미 처리됨
		return;
	}

	OnLoopCycleCompleted();

	if (bEndRequested || ShouldExitLoop())
	{
		if (false == bEndRequested)
		{
			bEndRequested     = true;
			CurrentEndMontage = GetAutoExitMontage();
			if (false == IsValid(CurrentEndMontage))
			{
				CurrentEndMontage = EndMontage.Get();
			}
		}
		PlayEndMontage();
	}
	else
	{
		if (LoopInertializationBlendTime > 0.f)
		{
			US1AnimInstance* AnimInst = GA.IsValid() ? GA->GetAnimInstanceForProgression() : nullptr;
			if (IsValid(AnimInst))
			{
				AnimInst->RequestInertialization(LoopInertializationBlendTime);
			}
		}
		PlayLoopMontage();
	}
}

void US1MontageProgression_Loop::OnEndMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CurrentEndMontage)
	{
		return;
	}

	if (GA.IsValid())
	{
		GA->RequestEndAbility(bInterrupted);
	}
}

void US1MontageProgression_Loop::OnLoopStartEventReceived(const FGameplayEventData* Payload)
{
	if (bEndRequested)
	{
		PlayEndMontage();
	}
	else
	{
		PlayLoopMontage();
		if (GA.IsValid())
		{
			GA->OnProgressionLoopStarted();
		}
	}
}

void US1MontageProgression_Loop::OnLoopCycleEventReceived(const FGameplayEventData* Payload)
{
	OnLoopCycleCompleted();

	if (bEndRequested || ShouldExitLoop())
	{
		if (false == bEndRequested)
		{
			bEndRequested     = true;
			CurrentEndMontage = GetAutoExitMontage();
			if (false == IsValid(CurrentEndMontage))
			{
				CurrentEndMontage = EndMontage.Get();
			}
		}
		PlayEndMontage();
	}
	else
	{
		if (LoopInertializationBlendTime > 0.f)
		{
			US1AnimInstance* AnimInst = GA.IsValid() ? GA->GetAnimInstanceForProgression() : nullptr;
			if (IsValid(AnimInst))
			{
				AnimInst->RequestInertialization(LoopInertializationBlendTime);
			}
		}
		PlayLoopMontage();
	}
}

void US1MontageProgression_Loop::OnLoopEndEventReceived(FGameplayEventData Payload)
{
	ExitLoop();
}
