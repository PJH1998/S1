// Fill out your copyright notice in the Description page of Project Settings.

#include "S1GA_Boss000_PhaseTransition.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AI/S1BossAIController.h"
#include "Character/Boss/Boss000/S1Boss_000.h"
#include "Data/S1AnimData.h"
#include "Tags/S1GameplayTags.h"

US1GA_Boss000_PhaseTransition::US1GA_Boss000_PhaseTransition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_PhaseTransition::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageData* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AS1Boss_000* Boss = Cast<AS1Boss_000>(ActorInfo->AvatarActor);
	if (nullptr == Boss)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 발동 즉시 펜딩만 해제(BT 재발동 방지). 무기 교체/페이즈 커밋은 몽타주 노티파이 타이밍에 처리한다.
	if (Boss->HasAuthority())
	{
		if (AS1BossAIController* BossAIController = Cast<AS1BossAIController>(Boss->GetController()))
		{
			BossAIController->SetPhaseTransitionPending(false);
		}
	}

	// Animation
	if (false == PlayAttackMontage(MontageSet->Montage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 정상 재생 -> 종료 시 최종 상태 보장.
	bCommitOnEnd = true;

	// 던지기/뽑기 노티파이 대기(JumpAttack의 Apex/Landed와 동일 패턴).
	UAbilityTask_WaitGameplayEvent* HideTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, S1EventTags::Event_Boss000_WeaponHide, nullptr, true, true);
	HideTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponHide);
	HideTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* DrawTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, S1EventTags::Event_Boss000_WeaponDraw, nullptr, true, true);
	DrawTask->EventReceived.AddDynamic(this, &ThisClass::OnWeaponDraw);
	DrawTask->ReadyForActivation();
}

void US1GA_Boss000_PhaseTransition::OnWeaponHide(FGameplayEventData Payload)
{
	if (AS1Boss_000* Boss = Cast<AS1Boss_000>(GetAvatarActorFromActorInfo()))
	{
		Boss->SetWeaponHidden(true);
	}
}

void US1GA_Boss000_PhaseTransition::OnWeaponDraw(FGameplayEventData Payload)
{
	AS1Boss_000* Boss = Cast<AS1Boss_000>(GetAvatarActorFromActorInfo());
	if (nullptr == Boss)
	{
		return;
	}

	Boss->SetActiveWeapon(EBossWeapon::Sword);
	Boss->SetWeaponHidden(false);

	if (AS1BossAIController* BossAIController = Cast<AS1BossAIController>(Boss->GetController()))
	{
		BossAIController->SetBossPhase(2);
	}
}

void US1GA_Boss000_PhaseTransition::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 중단/조기 종료 안전장치: 몽타주가 재생됐다면 무기가 숨김 상태로 멈추지 않도록 최종 상태를 보장한다(멱등).
	if (bCommitOnEnd)
	{
		if (AS1Boss_000* Boss = Cast<AS1Boss_000>(GetAvatarActorFromActorInfo()))
		{
			Boss->SetActiveWeapon(EBossWeapon::Sword);
			Boss->SetWeaponHidden(false);

			if (AS1BossAIController* BossAIController = Cast<AS1BossAIController>(Boss->GetController()))
			{
				BossAIController->SetBossPhase(2);
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}