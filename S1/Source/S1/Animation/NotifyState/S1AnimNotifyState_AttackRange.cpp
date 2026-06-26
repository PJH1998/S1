// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/S1AnimNotifyState_AttackRange.h"
#include "Character/S1Monster.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameplayEffect.h"
#include "S1Define.h"
#include "Effect/Decal/S1Decal.h"
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
	AS1Decal* Decal = DecalManager->ShowDecal(DecalPoolTag, Request.Location, Request.Rotation);
	if (AS1Decal_AttackRange* AttackRangeDecal = Cast<AS1Decal_AttackRange>(Decal))
	{
		AttackRangeDecal->ShowAttackRange(Request);
		ActiveDecals.FindOrAdd(MeshComp) = AttackRangeDecal;
	}

	HitActors.FindOrAdd(MeshComp).Reset();
}

void US1AnimNotifyState_AttackRange::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	ApplyRangeDamage(MeshComp);

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

	HitActors.Remove(MeshComp);
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

void US1AnimNotifyState_AttackRange::ApplyRangeDamage(USkeletalMeshComponent* MeshComp)
{
	if (bApplyDamage == false || DamageEffect == nullptr || MeshComp == nullptr)
	{
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(MeshComp->GetOwner());
	if (Monster == nullptr)
	{
		return;
	}

	if (false == Monster->HasAuthority())
	{
		return;
	}

	UWorld* World = Monster->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FS1AttackRangeDecalRequest Request = MakeRequest(MeshComp, 0.f);
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AttackRangeDamageOverlap), false, Monster);

	bool bHasOverlap = false;
	const FQuat QueryRotation = FRotator(0.f, Request.Rotation.Yaw, 0.f).Quaternion();
	switch (Shape)
	{
	case ES1AttackRangeDecalShape::Rectangle:
	{
		const FVector BoxExtent(
			FMath::Max(Length * 0.5f, 1.f),
			FMath::Max(Width * 0.5f, 1.f),
			FMath::Max(DamageHeight * 0.5f, 1.f));
		bHasOverlap = World->OverlapMultiByChannel(OverlapResults, Request.Location, QueryRotation, S1CollisionChannel::CC_Player, FCollisionShape::MakeBox(BoxExtent), QueryParams);
		break;
	}
	case ES1AttackRangeDecalShape::Cone:
	case ES1AttackRangeDecalShape::Circle:
	default:
		bHasOverlap = World->OverlapMultiByChannel(OverlapResults, Request.Location, FQuat::Identity, S1CollisionChannel::CC_Player, FCollisionShape::MakeSphere(FMath::Max(Radius, 1.f)), QueryParams);
		break;
	}

	if (bHasOverlap == false)
	{
		return;
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* TargetActor = OverlapResult.GetActor();
		if (TargetActor == nullptr || HasHitActor(MeshComp, TargetActor))
		{
			continue;
		}

		if (Shape == ES1AttackRangeDecalShape::Cone && IsInsideCone(Request, TargetActor->GetActorLocation()) == false)
		{
			continue;
		}

		AddHitActor(MeshComp, TargetActor);
		FHitResult DamageHitResult;
		DamageHitResult.HitObjectHandle = FActorInstanceHandle(TargetActor);
		DamageHitResult.Location = TargetActor->GetActorLocation();
		DamageHitResult.ImpactPoint = DamageHitResult.Location;
		Monster->ApplyAttackDamage(TargetActor, DamageEffect, DamageRatio, DamageHitResult);
	}
}

bool US1AnimNotifyState_AttackRange::IsInsideCone(const FS1AttackRangeDecalRequest& Request, const FVector& TargetLocation) const
{
	FVector ToTarget = TargetLocation - Request.Location;
	ToTarget.Z = 0.f;

	if (ToTarget.SizeSquared() > FMath::Square(FMath::Max(Radius, 1.f)))
	{
		return false;
	}

	if (ToTarget.Normalize() == false)
	{
		return true;
	}

	const FVector Forward = FRotator(0.f, Request.Rotation.Yaw, 0.f).Vector();
	const float HalfAngle = FMath::Clamp(Angle * 0.5f, 0.f, 180.f);
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(HalfAngle));
	return FVector::DotProduct(Forward, ToTarget) >= MinDot;
}

bool US1AnimNotifyState_AttackRange::HasHitActor(USkeletalMeshComponent* MeshComp, AActor* TargetActor) const
{
	const TArray<TWeakObjectPtr<AActor>>* HitActorList = HitActors.Find(MeshComp);
	if (HitActorList == nullptr)
	{
		return false;
	}

	for (const TWeakObjectPtr<AActor>& HitActor : *HitActorList)
	{
		if (HitActor.Get() == TargetActor)
		{
			return true;
		}
	}

	return false;
}

void US1AnimNotifyState_AttackRange::AddHitActor(USkeletalMeshComponent* MeshComp, AActor* TargetActor)
{
	HitActors.FindOrAdd(MeshComp).Add(TargetActor);
}
