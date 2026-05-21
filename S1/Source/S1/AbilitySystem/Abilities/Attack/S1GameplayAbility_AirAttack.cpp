// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_AirAttack.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1GameplayAbility_AirAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		// 점프 방향 속도 제거 — Root Motion만으로 이동하도록
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}

	if (UsedTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(UsedTag);
		}
	}

	// Super 내부에서 이전 몽타주 Stop → NotifyEnd(GravityScale=1.0)가 동기 실행되므로 반드시 이후에 설정
	SetGravityScale(AirGravityScale);
}

void US1GameplayAbility_AirAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SetGravityScale(1.f);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_AirAttack::SetGravityScale(float Scale) const
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->GravityScale = Scale;
	}
}
