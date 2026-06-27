// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "Animation/S1AnimInstance.h"
#include "Character/S1Character.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
US1GameplayAbility::US1GameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 서버 권위 실행 (적 GA와 동일 패턴) — 입력은 클라가 Server RPC로 전달, 로직은 서버에서만
	// 소유 클라 몽타주 가시성은 AS1Character::MulticastPlayMontage로 보완
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool US1GameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (false == Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
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
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return;
	}

	// 최초 호출 시에만 초기값 저장 — 이후 호출은 저장값 유지
	if (false == bGravityScaleSaved)
	{
		PrevGravityScale = Character->GetCharacterMovement()->GravityScale;
		bGravityScaleSaved = true;
	}

	// 복제 경유 — 소유 클라 CMC 예측이 서버와 일치 (호버 덜덜 방지)
	Character->SetReplicatedGravityScale(Scale);
}

void US1GameplayAbility::ResetGravityScale()
{
	if (false == bGravityScaleSaved)
	{
		return;
	}

	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		Character->SetReplicatedGravityScale(PrevGravityScale);
		bGravityScaleSaved = false;
	}
}

US1AnimInstance* US1GameplayAbility::GetAnimInstance() const
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return nullptr;
	}

	return Cast<US1AnimInstance>(Character->GetMesh()->GetAnimInstance());
}