// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/Combo/Air/S1GameplayAbility_ComboAttack_Air.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1GameplayAbility_ComboAttack_Air::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		// 점프 방향 속도 제거 — Root Motion만으로 이동하도록
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}

	// Super 내부에서 이전 몽타주 Stop → NotifyEnd(GravityScale=1.0)가 동기 실행되므로 반드시 이후에 설정
	SetGravityScale(AirGravityScale);
}

void US1GameplayAbility_ComboAttack_Air::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetGravityScale();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
