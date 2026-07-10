// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Evasion/Dash/S1GameplayAbility_Dash.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Progression/Directional/S1MontageProgression_Directional.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

US1GameplayAbility_Dash::US1GameplayAbility_Dash()
{
	// 항상 Directional 고정 — BP에서 안 채워도(까먹어도) 항상 유효
	MontageProgression = CreateDefaultSubobject<US1MontageProgression_Directional>(TEXT("MontageProgression"));
}

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

	// 입력 방향은 CMC Acceleration에서 — GetLastMovementInputVector()는 서버 autonomous proxy에서 0이라
	// 클라/서버가 다른 방향 애니를 선택(시뮬프록시 불일치). Acceleration은 ServerMove로 복제되어 일치
	const UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	const FVector InputDir = CMC ? CMC->GetCurrentAcceleration() : FVector::ZeroVector;
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
