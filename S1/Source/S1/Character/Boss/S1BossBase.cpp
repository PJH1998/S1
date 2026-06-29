// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/S1BossBase.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AI/S1BossAIController.h"

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
