// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/LooseGameplayTag/Air/S1AnimNotifyState_EnemyAir.h"

#include "Character/S1Monster.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1AnimNotifyState_EnemyAir::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AS1Monster* Monster = MeshComp ? Cast<AS1Monster>(MeshComp->GetOwner()) : nullptr;
	if (nullptr == Monster || false == Monster->HasAuthority())
	{
		return;
	}

	UCharacterMovementComponent* Movement = Monster->GetCharacterMovement();
	if (nullptr == Movement)
	{
		return;
	}

	// v = sqrt(2 * |g| * h) — 목표 정점 높이에 정확히 도달하는 발사 속도.
	const float GravityZ = FMath::Abs(Movement->GetGravityZ());
	const float LaunchZVel = FMath::Sqrt(2.f * GravityZ * ApexHeight);

	Monster->LaunchCharacter(FVector(0.f, 0.f, LaunchZVel), false, true);
	Monster->EnterAirborneState(ApexHeight);
}