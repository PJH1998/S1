// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Assault/S1GameplayAbility_Assault.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Character/Player/S1Player.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1GameplayTags.h"
#include "Weapon/S1Weapon.h"

void US1GameplayAbility_Assault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Super 이후 GravityScale 설정 (Super 내부에서 1.0으로 복원될 수 있음)
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->GravityScale = 0.f;
	}

	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->EnableHitCollision();
		}
	}

	if (MoveBeginEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveBeginEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnMoveBeginReceived);
		Task->ReadyForActivation();
	}

	if (MoveEndEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveEndEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnMoveEndReceived);
		Task->ReadyForActivation();
	}
}

void US1GameplayAbility_Assault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->GravityScale = 1.f;
	}

	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->DisableHitCollision();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Assault::OnMoveBeginReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		return;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		ComputeAssaultDirection(),
		AssaultSpeed,
		9999.f,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		false
	);
	MoveTask->ReadyForActivation();
}

void US1GameplayAbility_Assault::OnMoveEndReceived(FGameplayEventData Payload)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	if (false == IsValid(MontageProgression))
	{
		return;
	}

	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character) && Character->GetCharacterMovement()->IsFalling())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (IsValid(ASC) && false == ASC->HasMatchingGameplayTag(S1StateTags::State_Air))
		{
			ASC->AddLooseGameplayTag(S1StateTags::State_Air);
		}
		MontageProgression->OnBranchRequested(AirSection);
	}
	else
	{
		MontageProgression->OnBranchRequested(GroundSection);
	}
}

FVector US1GameplayAbility_Assault::ComputeAssaultDirection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return FVector::ForwardVector;
	}
	return Character->GetActorForwardVector();
}
