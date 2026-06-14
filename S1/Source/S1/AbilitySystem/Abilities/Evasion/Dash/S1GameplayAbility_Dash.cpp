// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Evasion/Dash/S1GameplayAbility_Dash.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Progression/Directional/S1MontageProgression_Directional.h"
#include "GameFramework/Character.h"

void US1GameplayAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 방향 캡처 + OrientRotation + MoveBegin/End 바인딩은 Evasion::ActivateAbility에서 처리
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(MontageProgression))
	{
		if (US1MontageProgression_Directional* DirProg = Cast<US1MontageProgression_Directional>(MontageProgression))
		{
			DirProg->SetDirection(ComputeDirection());
		}
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}
}

bool US1GameplayAbility_Dash::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (AttackAbilityGroupTags.IsEmpty())
	{
		return false;
	}

	if (false == TargetAbilityTags.HasAny(AttackAbilityGroupTags))
	{
		return false;
	}

	// 대쉬 중 공격 입력 — 윈도우 조건 없이 즉시 전환
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	return true;
}

ES1Direction US1GameplayAbility_Dash::ComputeDirection() const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return ES1Direction::Forward;
	}

	const FVector InputDir = Character->GetLastMovementInputVector();
	if (InputDir.IsNearlyZero())
	{
		return ES1Direction::Forward;
	}

	const FVector Forward = Character->GetActorForwardVector();
	const FVector Right   = Character->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(InputDir, Forward);
	const float RightDot   = FVector::DotProduct(InputDir, Right);
	const float Angle      = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));

	if (Angle >= -45.f && Angle <= 45.f)  { return ES1Direction::Forward; }
	if (Angle > 45.f  && Angle <= 135.f)  { return ES1Direction::Right; }
	if (Angle < -45.f && Angle >= -135.f) { return ES1Direction::Left; }
	return ES1Direction::Back;
}
