// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/S1BossBase.h"
#include "AbilitySystem/Attributes/S1BossSet.h"
#include "Component/S1BossLocomotionComponent.h"
#include "Component/S1DeathPresentationComponent.h"

ES1EnemyTier AS1BossBase::GetLockOnTier_Implementation()
{
	return ES1EnemyTier::Boss;
}

AS1BossBase::AS1BossBase()
	: Super()
{
	LocomotionComponent = CreateDefaultSubobject<US1BossLocomotionComponent>(TEXT("LocomotionComponent"));

	if (DeathPresentationComponent)
	{
		DeathPresentationComponent->PresentationMode = EDeathPresentation::AlphaFade;
		DeathPresentationComponent->FadeDuration = 2.f;
	}
}

void AS1BossBase::NotifyDeath()
{
	Super::NotifyDeath();

	if (bHasTarget)
	{
		NotifyHasTargetChanged(false);
	}
}

void AS1BossBase::NotifyHasTargetChanged(bool bInHasTarget)
{
	if (bHasTarget == bInHasTarget)
	{
		return;
	}

	bHasTarget = bInHasTarget;

	OnHasTargetChanged.Broadcast(this, bHasTarget);
}

US1BossSet* AS1BossBase::GetS1BossSet() const
{
	return Cast<US1BossSet>(AttributeSet);
}

const FName& AS1BossBase::GetBossName() const
{
	return BossName;
}
