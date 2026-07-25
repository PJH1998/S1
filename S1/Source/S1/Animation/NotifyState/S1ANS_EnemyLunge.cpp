// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1ANS_EnemyLunge.h"

#include "AI/S1AIController.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Character/S1Character.h"
#include "Character/S1Monster.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	AActor* ResolveCombatTarget(const AS1Monster* Monster)
	{
		if (Monster == nullptr)
		{
			return nullptr;
		}

		if (const AS1AIController* AIController = Cast<AS1AIController>(Monster->GetController()))
		{
			if (AActor* Target = AIController->GetTargetActor())
			{
				return Target;
			}
		}

		return UGameplayStatics::GetPlayerPawn(Monster, 0);
	}

	float GetCapsuleRadius(const AActor* Actor)
	{
		if (const UCapsuleComponent* Capsule = Actor ? Actor->FindComponentByClass<UCapsuleComponent>() : nullptr)
		{
			return Capsule->GetScaledCapsuleRadius();
		}

		return 0.f;
	}

	float GetEdgeDistance2D(const AActor* FromActor, const AActor* ToActor)
	{
		if (FromActor == nullptr || ToActor == nullptr)
		{
			return TNumericLimits<float>::Max();
		}

		const float CenterDistance = FVector::Dist2D(FromActor->GetActorLocation(), ToActor->GetActorLocation());
		return FMath::Max(0.f, CenterDistance - GetCapsuleRadius(FromActor) - GetCapsuleRadius(ToActor));
	}
}

US1ANS_EnemyLunge::US1ANS_EnemyLunge()
{
}

float US1ANS_EnemyLunge::LungeProgress(float NormalizedTime, float RushExponent, float TailLinearBlend)
{
	const float T = FMath::Clamp(NormalizedTime, 0.f, 1.f);
	const float Exp = FMath::Max(RushExponent, 1.f);
	const float EaseOut = 1.f - FMath::Pow(1.f - T, Exp);
	const float Blend = FMath::Clamp(TailLinearBlend, 0.f, 0.8f);
	return FMath::Lerp(EaseOut, T, Blend);
}

void US1ANS_EnemyLunge::ApplyMoveDelta(ACharacter* Character, const FVector& MoveDirection, float DeltaDist) const
{
	if (Character == nullptr || FMath::IsNearlyZero(DeltaDist))
	{
		return;
	}

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (Movement == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	Movement->SafeMoveUpdatedComponent(MoveDirection * DeltaDist, Character->GetActorRotation(), true, HitResult);
}

void US1ANS_EnemyLunge::ApplyLungeDelta(ACharacter* Character, const FS1EnemyLungeState& State, float PrevNormalizedTime, float CurrNormalizedTime) const
{
	if (Character == nullptr || FMath::IsNearlyZero(MoveDistance))
	{
		return;
	}

	const float DeltaDist = MoveDistance * (LungeProgress(CurrNormalizedTime, RushExponent, TailLinearBlend) - LungeProgress(PrevNormalizedTime, RushExponent, TailLinearBlend));
	ApplyMoveDelta(Character, State.MoveDirection, DeltaDist);
}

bool US1ANS_EnemyLunge::TryJumpToEndOnApproach(USkeletalMeshComponent* MeshComp, AS1Monster* Monster, FS1EnemyLungeState& State) const
{
	if (State.bHasJumpedToEnd || Monster == nullptr || MeshComp == nullptr || ApproachEndDistance <= 0.f || EndSectionName == NAME_None)
	{
		return false;
	}

	if (false == Monster->HasAuthority())
	{
		return false;
	}

	AActor* Target = ResolveCombatTarget(Monster);
	if (Target == nullptr)
	{
		return false;
	}

	if (GetEdgeDistance2D(Monster, Target) > ApproachEndDistance)
	{
		return false;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();
	if (Montage == nullptr)
	{
		return false;
	}

	if (AnimInstance->Montage_GetCurrentSection(Montage) == EndSectionName)
	{
		State.bHasJumpedToEnd = true;
		return true;
	}

	AnimInstance->Montage_JumpToSection(EndSectionName, Montage);
	State.bHasJumpedToEnd = true;
	return true;
}

void US1ANS_EnemyLunge::FinishLunge(USkeletalMeshComponent* MeshComp, ACharacter* Character, FS1EnemyLungeState& State)
{
	if (Character != nullptr && State.TotalDuration > KINDA_SMALL_NUMBER)
	{
		if (State.bHasJumpedToEnd == false)
		{
			const float PrevNormalizedTime = State.ElapsedTime / State.TotalDuration;
			ApplyLungeDelta(Character, State, PrevNormalizedTime, 1.f);
		}

		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			FVector Velocity = Movement->Velocity;
			Velocity.X = 0.f;
			Velocity.Y = 0.f;
			Movement->Velocity = Velocity;
		}

		if (bEnableGravity == false)
		{
			if (AS1Character* S1Char = Cast<AS1Character>(Character))
			{
				S1Char->SetReplicatedGravityScale(State.CachedGravityScale);
			}
		}
	}

	ActiveLunges.Remove(MeshComp);
}

void US1ANS_EnemyLunge::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp == nullptr || TotalDuration <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (FMath::IsNearlyZero(MoveDistance) && ApproachEndDistance <= 0.f)
	{
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(MeshComp->GetOwner());
	if (Monster == nullptr || Monster->IsDead())
	{
		return;
	}

	// 이동 계산은 서버 권한에서만 — 클라는 캐릭터 무브먼트 복제로 따라온다(S1ANS_WallKickDash와 동일 패턴).
	if (false == Monster->HasAuthority())
	{
		return;
	}

	ACharacter* Character = Monster;
	if (AAIController* AIController = Cast<AAIController>(Monster->GetController()))
	{
		AIController->StopMovement();
	}

	FVector MoveDirection = Character->GetActorForwardVector();
	MoveDirection.Z = 0.f;
	if (MoveDirection.Normalize() == false)
	{
		return;
	}

	FS1EnemyLungeState& State = ActiveLunges.FindOrAdd(MeshComp);
	State.ElapsedTime = 0.f;
	State.TotalDuration = TotalDuration;
	State.MoveDirection = MoveDirection;
	State.CachedGravityScale = 1.f;
	State.bHasJumpedToEnd = false;

	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		State.CachedGravityScale = Movement->GravityScale;
		if (bEnableGravity == false)
		{
			Monster->SetReplicatedGravityScale(0.f);
		}
	}
}

void US1ANS_EnemyLunge::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	FS1EnemyLungeState* State = ActiveLunges.Find(MeshComp);
	if (State == nullptr)
	{
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(MeshComp->GetOwner());
	if (Monster == nullptr || Monster->IsDead())
	{
		FinishLunge(MeshComp, Monster, *State);
		return;
	}

	const float PrevNormalizedTime = State->ElapsedTime / State->TotalDuration;
	State->ElapsedTime += FrameDeltaTime;
	const float CurrNormalizedTime = FMath::Min(State->ElapsedTime / State->TotalDuration, 1.f);

	if (TryJumpToEndOnApproach(MeshComp, Monster, *State))
	{
		return;
	}

	ApplyLungeDelta(Monster, *State, PrevNormalizedTime, CurrNormalizedTime);
	TryJumpToEndOnApproach(MeshComp, Monster, *State);
}

void US1ANS_EnemyLunge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	FS1EnemyLungeState* State = ActiveLunges.Find(MeshComp);
	if (State == nullptr)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	FinishLunge(MeshComp, Character, *State);
}
