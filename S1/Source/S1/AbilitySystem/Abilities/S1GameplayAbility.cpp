// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "Animation/S1AnimInstance.h"
#include "Character/S1Character.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
US1GameplayAbility::US1GameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool US1GameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}

	return true;
}

void US1GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void US1GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bGravityScaleSaved = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility::SetGravityScale(float Scale)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (nullptr == Character)
	{
		return;
	}

	// 최초 호출 시에만 초기값 저장 — 이후 호출은 저장값 유지
	if (false == bGravityScaleSaved)
	{
		PrevGravityScale = Character->GetCharacterMovement()->GravityScale;
		bGravityScaleSaved = true;
	}

	Character->GetCharacterMovement()->GravityScale = Scale;
}

void US1GameplayAbility::ResetGravityScale()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->GravityScale = PrevGravityScale;
	}
}

US1AnimInstance* US1GameplayAbility::GetAnimInstance() const
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (nullptr == Character)
	{
		return nullptr;
	}

	return Cast<US1AnimInstance>(Character->GetMesh()->GetAnimInstance());
}