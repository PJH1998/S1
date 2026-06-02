// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/S1AnimInstance_BossBase.h"
#include "Character/Boss/S1BossBase.h"
#include "Character/S1Monster.h"
#include "Component/S1BossLocomotionComponent.h"

void US1AnimInstance_BossBase::NativeUpdateAnimation(float DeltaSeconds)
{
	if (bIsDeadAnim)
	{
		Super::NativeUpdateAnimation(DeltaSeconds);
		bMove = false;
		GroundSpeed = 0.f;
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(GetOwningActor()))
	{
		if (Monster->IsDead())
		{
			Super::NativeUpdateAnimation(DeltaSeconds);
			bMove = false;
			GroundSpeed = 0.f;
			return;
		}
	}

	Super::NativeUpdateAnimation(DeltaSeconds);
	if (LocomotionMode == EBossLocomotionMode::Turn)
	{
		bMove = false;
	}
	else if (LocomotionMode != EBossLocomotionMode::None)
	{
		bMove = LocomotionPhase != EBossLocomotionPhase::End;
	}
	else
	{
		bMove = GroundSpeed > 3.f;
	}
}

void US1AnimInstance_BossBase::BeginApproach(EBossLocomotionMode InMode)
{
	LocomotionMode = InMode;
	LocomotionPhase = EBossLocomotionPhase::Start;
	TurnDirection = EBossTurnDirection::None;
	bLocomotionLoop = false;
}

void US1AnimInstance_BossBase::BeginTurn(EBossTurnDirection InDirection)
{
	LocomotionMode = EBossLocomotionMode::Turn;
	LocomotionPhase = EBossLocomotionPhase::Start;
	TurnDirection = InDirection;
	bLocomotionLoop = false;
}

void US1AnimInstance_BossBase::RequestStop()
{
	if (LocomotionMode == EBossLocomotionMode::None)
	{
		return;
	}
	LocomotionPhase = EBossLocomotionPhase::End;
	bLocomotionLoop = false;
}

void US1AnimInstance_BossBase::ResetLocomotion()
{
	LocomotionMode = EBossLocomotionMode::None;
	LocomotionPhase = EBossLocomotionPhase::None;
	TurnDirection = EBossTurnDirection::None;
	bLocomotionLoop = false;
}

void US1AnimInstance_BossBase::SetDeadAnimState(bool bInDead)
{
	bIsDeadAnim = bInDead;
}

bool US1AnimInstance_BossBase::IsLocomotionEndPlaying() const
{
	return LocomotionMode != EBossLocomotionMode::None && LocomotionPhase == EBossLocomotionPhase::End;
}

void US1AnimInstance_BossBase::AnimNotify_LocomotionLoopStart()
{
	if (bIsDeadAnim)
	{
		return;
	}

	if (LocomotionMode == EBossLocomotionMode::None || LocomotionPhase != EBossLocomotionPhase::Start)
	{
		return;
	}
	bLocomotionLoop = true;
	LocomotionPhase = EBossLocomotionPhase::Loop;
}

void US1AnimInstance_BossBase::AnimNotify_LocomotionEndFinished()
{
	if (bIsDeadAnim)
	{
		return;
	}

	ResetLocomotion();
	if (AS1BossBase* Boss = Cast<AS1BossBase>(GetOwningActor()))
	{
		if (US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent())
		{
			LocomotionComponent->NotifyLocomotionStopFinished();
		}
	}
}

void US1AnimInstance_BossBase::AnimNotify_TurnFinished()
{
	if (bIsDeadAnim)
	{
		return;
	}

	ResetLocomotion();
	if (AS1BossBase* Boss = Cast<AS1BossBase>(GetOwningActor()))
	{
		if (US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent())
		{
			LocomotionComponent->NotifyTurnFinished();
		}
	}
}

void US1AnimInstance_BossBase::AnimNotify_DeathFinished()
{
	if (!bIsDeadAnim)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(GetOwningActor()))
	{
		Monster->OnDeathAnimSequenceEnded();
		Monster->BeginDeathPresentation();
	}
}

void US1AnimInstance_BossBase::AnimNotify_BeginDeathPresentation()
{
	if (!bIsDeadAnim)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(GetOwningActor()))
	{
		Monster->BeginDeathPresentation();
	}
}
