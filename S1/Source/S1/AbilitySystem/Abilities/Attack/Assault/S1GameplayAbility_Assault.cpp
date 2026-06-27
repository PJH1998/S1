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

US1GameplayAbility_Assault::US1GameplayAbility_Assault()
{
	// 루트모션 이동 예측을 위해 LocalPredicted — AbilityTask_ApplyRootMotion이 소유 클라에서 예측 실행
	// ⚠️ §39: BP에 저장된 값이 C++ 기본값을 이김 → GA_Assault BP Class Defaults에서도 Local Predicted 설정 필요
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 자체 중력 제어(ActivateAbility에서 GravityScale=0) → 이동 루트모션은 중력 비관여
	bMoveEnableGravity = false;
}

FVector US1GameplayAbility_Assault::GetCapturedMoveDirection() const
{
	return CapturedAssaultDirection;
}

void US1GameplayAbility_Assault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Super 전에 캡처 — Pitch 포함한 카메라 3D 방향 (캐릭터 회전은 안 함)
	if (const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		CapturedAssaultDirection = Character->GetControlRotation().Vector();
	}

	bBranchRequested = false;

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// GravityScale 비활성화(복제 경유) + 기존 관성 제거
	SetGravityScale(0.f);
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}

	// 이동(루트모션)은 GA_Action::PlayAbilityMontage→ApplyMontageRootMotion가 활성화 시점(예측 스코프)에 처리
	// 히트 콜리전은 이동 구간(OnMoveBeginReceived)에서만 활성화
}

void US1GameplayAbility_Assault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	ResetGravityScale();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Assault::OnMoveBeginReceived(const FGameplayEventData* Payload)
{
	// 이동(루트모션)은 활성화 시점에 ApplyMontageRootMotion가 이미 생성(방향은 GetCapturedMoveDirection 3D override)
	// 여기선 히트 콜리전만 (서버 권위) — 이동 구간에만 활성화
	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->EnableHitCollision(AssaultAtkScale, AssaultHitStrengthTag);
		}
	}
}

void US1GameplayAbility_Assault::OnMoveEndReceived(const FGameplayEventData* Payload)
{
	// PlayAnimMontage(EndMontage)가 BeginMontage를 중단할 때 MoveEvent NotifyEnd가 동기 재발화됨 → 중복 차단
	if (bBranchRequested)
	{
		return;
	}
	bBranchRequested = true;

	// 이동 구간 종료 — 히트 콜리전 비활성화
	// (루트모션 task는 유한 Duration으로 스스로 종료 — 여기서 끊으면 클라/서버 타이밍 desync)
	if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->DisableHitCollision();
		}
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

