// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/S1AnimNotifyState_AttackRange.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "System/S1DecalManager.h"

void US1AnimNotifyState_AttackRange::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	US1DecalManager* DecalManager = US1DecalManager::Get(MeshComp);
	if (DecalManager == nullptr)
	{
		return;
	}

	FS1AttackRangeDecalRequest Request = MakeRequest(MeshComp, TotalDuration);
	AS1Decal_AttackRange* Decal = DecalManager->ShowAttackRangeDecal(Request);
	if (Decal)
	{
		ActiveDecals.FindOrAdd(MeshComp) = Decal;
	}
}

void US1AnimNotifyState_AttackRange::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	TWeakObjectPtr<AS1Decal_AttackRange> DecalPtr;
	if (ActiveDecals.RemoveAndCopyValue(MeshComp, DecalPtr))
	{
		if (AS1Decal_AttackRange* Decal = DecalPtr.Get())
		{
			if (US1DecalManager* DecalManager = US1DecalManager::Get(MeshComp))
			{
				DecalManager->HideDecal(Decal);
			}
		}
	}
}

FS1AttackRangeDecalRequest US1AnimNotifyState_AttackRange::MakeRequest(USkeletalMeshComponent* MeshComp, float TotalDuration) const
{
	FS1AttackRangeDecalRequest Request;
	if (MeshComp == nullptr)
	{
		return Request;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return Request;
	}

	const FTransform OwnerTransform = Owner->GetActorTransform();
	FVector DecalLocation = OwnerTransform.TransformPosition(LocationOffset);
	if (bProjectToGround)
	{
		const FVector TraceStart = DecalLocation + FVector(0.f, 0.f, GroundTraceHeight);
		const FVector TraceEnd = DecalLocation - FVector(0.f, 0.f, GroundTraceDistance);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AttackRangeDecalGroundTrace), false, Owner);

		if (UWorld* World = Owner->GetWorld())
		{
			if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
			{
				DecalLocation = HitResult.ImpactPoint;
			}
		}
	}

	Request.Shape = Shape;
	Request.Location = DecalLocation;
	Request.Rotation = FRotator(-90.f, Owner->GetActorRotation().Yaw + YawOffset, 0.f);
	Request.Radius = Radius;
	Request.Angle = Angle;
	Request.Width = Width;
	Request.Length = Length;
	Request.Duration = TotalDuration;
	Request.ProjectionDepth = bProjectToGround ? FMath::Min(ProjectionDepth, 64.f) : ProjectionDepth;
	Request.Color = Color;

	return Request;
}
