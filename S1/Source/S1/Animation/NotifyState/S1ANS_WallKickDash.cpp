// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1ANS_WallKickDash.h"

#include "AIController.h"
#include "Character/Boss/Boss000/S1Boss_000.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void US1ANS_WallKickDash::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp == nullptr || TotalDuration <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	AS1Boss_000* Monster = Cast<AS1Boss_000>(MeshComp->GetOwner());
	if (Monster == nullptr || Monster->IsDead())
	{
		return;
	}

	// 타깃은 서버에서만 설정 → 이동도 서버 권한에서만. 클라는 캐릭터 무브먼트 복제로 따라온다.
	if (false == Monster->HasAuthority())
	{
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(Monster->GetController()))
	{
		AIController->StopMovement();
	}

	FS1WallKickDashState& State = ActiveDashes.FindOrAdd(MeshComp);
	State.StartLocation = Monster->GetActorLocation();
	State.TargetLocation = Monster->GetWallKickTarget();
	State.ElapsedTime = 0.f;
	State.TotalDuration = TotalDuration;

	if (UCharacterMovementComponent* Movement = Monster->GetCharacterMovement())
	{
		// 중력은 어빌리티(GA)가 패턴 단위로 관리. 여기선 잔여 속도만 제거해 드리프트 방지.
		Movement->Velocity = FVector::ZeroVector;
	}

	if (bFaceTarget)
	{
		FVector ToTarget = State.TargetLocation - State.StartLocation;
		ToTarget.Z = 0.f;
		if (ToTarget.Normalize())
		{
			FRotator NewRotation = Monster->GetActorRotation();
			NewRotation.Yaw = ToTarget.Rotation().Yaw;
			Monster->SetActorRotation(NewRotation);
		}
	}
}

void US1ANS_WallKickDash::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	FS1WallKickDashState* State = ActiveDashes.Find(MeshComp);
	if (State == nullptr)
	{
		return;
	}

	AS1Boss_000* Monster = Cast<AS1Boss_000>(MeshComp->GetOwner());
	if (Monster == nullptr || Monster->IsDead())
	{
		Finish(MeshComp, Monster, *State);
		return;
	}

	State->ElapsedTime += FrameDeltaTime;
	const float NormalizedTime = (State->TotalDuration > KINDA_SMALL_NUMBER) ? FMath::Clamp(State->ElapsedTime / State->TotalDuration, 0.f, 1.f) : 1.f;
	const float Eased = 1.f - FMath::Pow(1.f - NormalizedTime, FMath::Max(MoveExponent, 1.f));

	const FVector DesiredLocation = FMath::Lerp(State->StartLocation, State->TargetLocation, Eased);
	MoveTo(Monster, DesiredLocation);
}

void US1ANS_WallKickDash::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	FS1WallKickDashState* State = ActiveDashes.Find(MeshComp);
	if (State == nullptr)
	{
		return;
	}

	AS1Boss_000* Monster = Cast<AS1Boss_000>(MeshComp->GetOwner());
	Finish(MeshComp, Monster, *State);
}

void US1ANS_WallKickDash::MoveTo(AS1Boss_000* Monster, const FVector& DesiredLocation) const
{
	if (Monster == nullptr)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Monster->GetCharacterMovement();
	if (Movement == nullptr)
	{
		return;
	}

	const FVector Delta = DesiredLocation - Monster->GetActorLocation();
	if (Delta.IsNearlyZero())
	{
		return;
	}

	FHitResult HitResult;
	Movement->SafeMoveUpdatedComponent(Delta, Monster->GetActorRotation(), true, HitResult);
}

void US1ANS_WallKickDash::Finish(USkeletalMeshComponent* MeshComp, AS1Boss_000* Monster, FS1WallKickDashState& State)
{
	if (Monster != nullptr)
	{
		// 거의 완료된 경우에만 타깃에 정확히 스냅한다(히트리액트 등으로 중간에 끊기면 제자리에서 멈춤).
		const bool bCompleted = (State.TotalDuration > KINDA_SMALL_NUMBER) && (State.ElapsedTime / State.TotalDuration >= 0.95f);
		if (bCompleted)
		{
			MoveTo(Monster, State.TargetLocation);
		}

		if (UCharacterMovementComponent* Movement = Monster->GetCharacterMovement())
		{
			Movement->Velocity = FVector::ZeroVector;
		}
	}

	ActiveDashes.Remove(MeshComp);
}
