// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/S1AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1LogChannels.h"

US1MontageProgression_Loop::US1MontageProgression_Loop()
{
	LoopStartEventTag = S1EventTags::Event_LoopStart;
	LoopCycleEventTag = S1EventTags::Event_LoopCycle;
}

void US1MontageProgression_Loop::OnActivated()
{
	bEndRequested         = false;
	bInLoopPhase          = false;
	CurrentEndSectionName = EndSectionName;

	if (false == GA.IsValid())
	{
		return;
	}

	if (false == ::IsValid(SectionMontage) || LoopSectionName.IsNone())
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

	const bool bHasStart = false == StartSectionName.IsNone();
	StartMontageTask(bHasStart ? StartSectionName : LoopSectionName);
	if (false == ::IsValid(ActiveMontageTask))
	{
		return;		// StartMontageTask가 이미 EndAbility를 요청했다
	}

	// 섹션 흐름을 코드가 정한다 - 애셋에 저작된 Next 링크에 의존하지 않는다.
	// Loop -> Loop 가 사이클 반복을 자동 처리하므로 재생 호출을 매 사이클 하지 않아도 된다
	if (bHasStart)
	{
		GA->SetMontageNextSection(StartSectionName, LoopSectionName);
	}
	GA->SetMontageNextSection(LoopSectionName, LoopSectionName);
	GA->SetMontageNextSection(EndSectionName, NAME_None);

	// 사이클 경계 통지는 활성화 시점에 걸어둔다 — Loop 진입 통지(LoopStartEventTag)가 비어 있어도
	// 사이클을 놓치지 않도록
	SubscribeLoopCycle();

	if (bHasStart && LoopStartEventTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopStartEventTag)
				.AddUObject(this, &ThisClass::OnLoopStartEventReceived);
		}
	}
	else
	{
		// Start가 없거나 진입 통지가 설정되지 않았다 → 처음부터 Loop 단계로 간주
		EnterLoopPhase();
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
		if (UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo())
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

	ReleaseMontageTask();

	bEndRequested         = false;
	bInLoopPhase          = false;
	CurrentEndSectionName = NAME_None;
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

void US1MontageProgression_Loop::ExitLoop(FName TargetEndSection, bool bImmediate)
{
	if (bEndRequested)
	{
		return;
	}

	bEndRequested         = true;
	CurrentEndSectionName = TargetEndSection.IsNone() ? EndSectionName : TargetEndSection;

	if (false == GA.IsValid() || CurrentEndSectionName.IsNone())
	{
		return;
	}

	if (bImmediate && bInLoopPhase)
	{
		// Loop 단계에서만 즉시 점프 — Start 재생 중이면 아래 Next 변경이 처리한다
		RequestSectionBlend();
		GA->JumpToMontageSection(SectionMontage, CurrentEndSectionName);
		return;
	}

	// 현재 섹션이 끝나면 End로 흐르게 한다. Start 중에 요청이 들어왔으면 Loop를 건너 바로 End로
	GA->SetMontageNextSection(LoopSectionName, CurrentEndSectionName);
	if (false == StartSectionName.IsNone())
	{
		GA->SetMontageNextSection(StartSectionName, CurrentEndSectionName);
	}
}

void US1MontageProgression_Loop::RequestSectionBlend()
{
	if (LoopInertializationBlendTime <= 0.f || false == GA.IsValid())
	{
		return;
	}

	if (US1AnimInstance* AnimInst = GA->GetAnimInstanceForProgression())
	{
		AnimInst->RequestInertialization(LoopInertializationBlendTime);
	}
}

void US1MontageProgression_Loop::StartMontageTask(FName StartSection)
{
	ReleaseMontageTask();

	ActiveMontageTask = GA->PlayAbilityMontageTask(SectionMontage, StartSection);
	if (false == ::IsValid(ActiveMontageTask))
	{
		GA->RequestEndAbility(true);
		return;
	}

	// 네 개를 다 붙여야 중단 시 어빌리티가 끝난다 — OnCompleted만 걸면 인터럽트에서 멈춘다.
	// 섹션 반복/점프는 몽타주를 끊지 않으므로 이 태스크는 End 섹션이 끝날 때까지 산다
	ActiveMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
	ActiveMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageAborted);
	ActiveMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageAborted);

	ActiveMontageTask->ReadyForActivation();
}

void US1MontageProgression_Loop::ReleaseMontageTask()
{
	if (false == ::IsValid(ActiveMontageTask))
	{
		ActiveMontageTask = nullptr;
		return;
	}

	ActiveMontageTask->OnCompleted.RemoveAll(this);
	ActiveMontageTask->OnBlendOut.RemoveAll(this);
	ActiveMontageTask->OnInterrupted.RemoveAll(this);
	ActiveMontageTask->OnCancelled.RemoveAll(this);
	ActiveMontageTask->EndTask();
	ActiveMontageTask = nullptr;
}

void US1MontageProgression_Loop::EnterLoopPhase()
{
	if (bInLoopPhase)
	{
		return;
	}

	bInLoopPhase = true;

	if (GA.IsValid())
	{
		GA->OnProgressionLoopStarted();
	}
}

void US1MontageProgression_Loop::SubscribeLoopCycle()
{
	if (false == LoopCycleEventTag.IsValid() || false == GA.IsValid())
	{
		LOG_WARNING(TEXT("LoopCycleEventTag 미설정 — 섹션 모드에선 사이클 경계를 알 수 없어 Loop를 탈출하지 못한다 (%s)"),
			*GetNameSafe(SectionMontage));
		return;
	}

	if (UAbilitySystemComponent* ASC = GA->GetAbilitySystemComponentFromActorInfo())
	{
		auto& EventDelegate = ASC->GenericGameplayEventCallbacks.FindOrAdd(LoopCycleEventTag);
		EventDelegate.RemoveAll(this);
		EventDelegate.AddUObject(this, &ThisClass::OnLoopCycleEventReceived);
	}
}

void US1MontageProgression_Loop::OnLoopStartEventReceived(const FGameplayEventData* Payload)
{
	// 전환 자체는 섹션 Next가 처리한다 — 여기서는 Loop 진입 통지만.
	// 이미 탈출이 요청된 상태면 Start → End로 흐르므로 Loop 단계에 들어가지 않는다
	if (false == bEndRequested)
	{
		EnterLoopPhase();
	}
}

void US1MontageProgression_Loop::OnLoopCycleEventReceived(const FGameplayEventData* Payload)
{
	// Charge류 전용 Notify(SpawnLoopCycleEffect)만 InstigatorTags에 EffectTag를 실어 보냄 — 일반 SendGameplayEvent는 비워둠(Invalid)
	FGameplayTag EffectTag;
	if (nullptr != Payload && Payload->InstigatorTags.Num() > 0)
	{
		EffectTag = Payload->InstigatorTags.GetByIndex(0);
	}

	// Start 없이 Loop부터 시작했거나 진입 통지를 놓친 경우 보정
	EnterLoopPhase();

	OnLoopCycleCompleted(EffectTag);

	if (false == GA.IsValid())
	{
		return;
	}

	if (bEndRequested || ShouldExitLoop())
	{
		if (false == bEndRequested)
		{
			bEndRequested         = true;
			CurrentEndSectionName = GetAutoExitSectionName();
			if (CurrentEndSectionName.IsNone())
			{
				CurrentEndSectionName = EndSectionName;
			}
		}

		// 현재 Loop 사이클이 끝나면 End로 흐른다. 섹션 전환은 하드컷이라 포즈를 이어준다
		RequestSectionBlend();
		GA->SetMontageNextSection(LoopSectionName, CurrentEndSectionName);
		return;
	}

	// Next가 Loop 그대로라 자동 반복된다 — 되돌아가는 지점이 하드컷이므로 포즈만 이어준다
	RequestSectionBlend();
}

void US1MontageProgression_Loop::OnLoopEndEventReceived(FGameplayEventData Payload)
{
	ExitLoop(NAME_None, bImmediateExitOnLoopEnd);
}

void US1MontageProgression_Loop::OnMontageFinished()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(false);
	}
}

void US1MontageProgression_Loop::OnMontageAborted()
{
	if (GA.IsValid())
	{
		GA->RequestEndAbility(true);
	}
}
