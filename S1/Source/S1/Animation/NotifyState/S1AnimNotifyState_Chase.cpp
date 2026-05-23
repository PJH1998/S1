// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/S1AnimNotifyState_Chase.h"

#include "AI/S1AIController.h"
#include "Character/S1Monster.h"
#include "Engine/World.h"

void US1AnimNotifyState_Chase::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(Owner);
	if (Monster == nullptr)
	{
		return;
	}

	AS1AIController* AIController = Cast<AS1AIController>(Monster->GetController());
	if (AIController == nullptr)
	{
		return;
	}

	AActor* Target = AIController->GetTargetActor();
	if (Target == nullptr)
	{
		return;
	}

	FVector Direction = Target->GetActorLocation() - Owner->GetActorLocation();
	Direction.Z = 0.f;

	if (Direction.Normalize() == false)
	{
		return;
	}

	FRotator CurrentRot = Owner->GetActorRotation();
	FRotator DestRot = Direction.Rotation();

	FRotator NextRot = FMath::RInterpConstantTo(CurrentRot, DestRot, FrameDeltaTime, RotationSpeed);

	Owner->SetActorRotation(FRotator(0.f, NextRot.Yaw, 0.f));
}
