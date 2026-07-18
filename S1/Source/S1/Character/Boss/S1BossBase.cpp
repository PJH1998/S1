// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/S1BossBase.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AI/S1BossAIController.h"
#include "System/S1CombatFeedbackSubsystem.h"
#include "System/S1SoundManager.h"
#include "Tags/World/S1GameplayTags_World.h"

ES1EnemyTier AS1BossBase::GetLockOnTier_Implementation()
{
	return ES1EnemyTier::Boss;
}

AS1BossBase::AS1BossBase()
	: Super()
{
	// 보스는 페이즈 전환 블랙보드를 다루는 전용 컨트롤러를 사용한다.
	AIControllerClass = AS1BossAIController::StaticClass();
}

void AS1BossBase::RequestPhaseTransition()
{
	if (false == HasAuthority() || bPhaseTransitionTriggered)
	{
		return;
	}
	bPhaseTransitionTriggered = true;

	if (AS1BossAIController* BossAIController = Cast<AS1BossAIController>(GetController()))
	{
		BossAIController->SetPhaseTransitionPending(true);
	}
}

US1BossSet* AS1BossBase::GetS1BossSet() const
{
	return Cast<US1BossSet>(AttributeSet);
}

const FName& AS1BossBase::GetBossName() const
{
	return BossName;
}

void AS1BossBase::MulticastNotifyGuardBlocked_Implementation(const FVector& InBlockLocation)
{
	if (UWorld* World = GetWorld())
	{
		if (US1CombatFeedbackSubsystem* CombatFeedback = World->GetSubsystem<US1CombatFeedbackSubsystem>())
		{
			CombatFeedback->ShowBlockNumber(InBlockLocation);
		}

		// 가드 차단음 — 각 머신 로컬 재생(데디서버 제외는 SoundManager가 처리).
		if (US1SoundManager* SoundManager = World->GetSubsystem<US1SoundManager>())
		{
			SoundManager->PlaySoundAtLocationByTag(S1SoundTags::Sound_Boss_GuardBlock, InBlockLocation);
		}
	}
}
