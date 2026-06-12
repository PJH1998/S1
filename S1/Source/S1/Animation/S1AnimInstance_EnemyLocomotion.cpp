// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "Character/S1Monster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "GameFramework/Character.h"

void US1AnimInstance_EnemyLocomotion::NativeUpdateAnimation(float DeltaSeconds)
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
	if (IsPlayingTurnMontage())
	{
		bMove = false;
	}
	else if (LocomotionMode != EEnemyLocomotionMode::None)
	{
		bMove = LocomotionPhase != EEnemyLocomotionPhase::End;
	}
	else
	{
		bMove = GroundSpeed > 3.f;
	}
}

void US1AnimInstance_EnemyLocomotion::BeginApproach(EEnemyLocomotionMode InMode)
{
	LocomotionMode = InMode;
	LocomotionPhase = EEnemyLocomotionPhase::Start;
	bLocomotionLoop = false;
}

bool US1AnimInstance_EnemyLocomotion::PlayTurnMontage(UAnimMontage* Montage, FName StartSection)
{
	if (Montage == nullptr || StartSection.IsNone())
	{
		return false;
	}

	StopTurnMontage();

	ACharacter* OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter == nullptr)
	{
		return false;
	}

	const float MontageLength = OwnerCharacter->PlayAnimMontage(Montage, 1.f, StartSection);
	if (MontageLength <= 0.f)
	{
		return false;
	}

	ActiveTurnMontage = Montage;
	return true;
}

void US1AnimInstance_EnemyLocomotion::StopTurnMontage()
{
	if (ActiveTurnMontage.IsValid())
	{
		Montage_Stop(0.1f, ActiveTurnMontage.Get());
		ActiveTurnMontage = nullptr;
	}
}

bool US1AnimInstance_EnemyLocomotion::IsPlayingTurnMontage() const
{
	return ActiveTurnMontage.IsValid() && Montage_IsPlaying(ActiveTurnMontage.Get());
}

void US1AnimInstance_EnemyLocomotion::RequestStop()
{
	if (LocomotionMode == EEnemyLocomotionMode::None)
	{
		return;
	}
	LocomotionPhase = EEnemyLocomotionPhase::End;
	bLocomotionLoop = false;
}

void US1AnimInstance_EnemyLocomotion::ResetLocomotion()
{
	StopTurnMontage();
	LocomotionMode = EEnemyLocomotionMode::None;
	LocomotionPhase = EEnemyLocomotionPhase::None;
	bLocomotionLoop = false;
}

void US1AnimInstance_EnemyLocomotion::SetDeadAnimState(bool bInDead)
{
	bIsDeadAnim = bInDead;
}

bool US1AnimInstance_EnemyLocomotion::IsLocomotionEndPlaying() const
{
	return LocomotionMode != EEnemyLocomotionMode::None && LocomotionPhase == EEnemyLocomotionPhase::End;
}

void US1AnimInstance_EnemyLocomotion::AnimNotify_LocomotionLoopStart()
{
	if (bIsDeadAnim)
	{
		return;
	}

	if (LocomotionMode == EEnemyLocomotionMode::None || LocomotionPhase != EEnemyLocomotionPhase::Start)
	{
		return;
	}
	bLocomotionLoop = true;
	LocomotionPhase = EEnemyLocomotionPhase::Loop;
}

void US1AnimInstance_EnemyLocomotion::AnimNotify_LocomotionEndFinished()
{
	if (bIsDeadAnim)
	{
		return;
	}

	ResetLocomotion();
	if (AS1Monster* Monster = Cast<AS1Monster>(GetOwningActor()))
	{
		if (US1EnemyLocomotionComponent* LocomotionComponent = Monster->GetLocomotionComponent())
		{
			LocomotionComponent->NotifyLocomotionStopFinished();
		}
	}
}

void US1AnimInstance_EnemyLocomotion::AnimNotify_TurnFinished()
{
	if (bIsDeadAnim)
	{
		return;
	}

	StopTurnMontage();
}

void US1AnimInstance_EnemyLocomotion::AnimNotify_DeathFinished()
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

void US1AnimInstance_EnemyLocomotion::AnimNotify_BeginDeathPresentation()
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