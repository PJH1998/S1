// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Assault/S1GameplayAbility_Assault.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilitySystemComponent.h"
#include "Character/Player/S1Player.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/S1GameplayTags.h"
#include "Weapon/S1Weapon.h"

void US1GameplayAbility_Assault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Super 전에 캡처 — Pitch 포함한 카메라 3D 방향 (캐릭터 회전은 안 함)
	if (const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		CapturedAssaultDirection = Character->GetControlRotation().Vector();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// GravityScale 비활성화 + 기존 관성 제거
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		CMC->GravityScale = 0.f;
		CMC->Velocity = FVector::ZeroVector;
	}

	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->EnableHitCollision();
		}
	}

	// Move 이벤트 바인딩은 GA_Action::ActivateAbility에서 처리
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

void US1GameplayAbility_Assault::OnMoveBeginReceived(const FGameplayEventData* Payload)
{
	if (IsValid(MoveTask))
	{
		return;
	}

	const float Impulse = Payload ? Payload->EventMagnitude : 0.f;
	if (FMath::IsNearlyZero(Impulse))
	{
		return;
	}

	// 점프 등 기존 관성 제거 — ConstantForce가 기존 Velocity에 누적되는 것 방지
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		CapturedAssaultDirection,
		Impulse,
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

void US1GameplayAbility_Assault::OnMoveEndReceived(const FGameplayEventData* Payload)
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
		MontageProgression->OnBranchRequested(AirEndTag);
	}
	else
	{
		MontageProgression->OnBranchRequested(GroundEndTag);
	}
}

