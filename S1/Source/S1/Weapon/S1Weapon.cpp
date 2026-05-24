// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Components/SkeletalMeshComponent.h"

AS1Weapon::AS1Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void AS1Weapon::DoLineTrace(TArray<FHitResult>& OutHitResults)
{
	if (false == IsValid(WeaponMesh))
	{
		return;
	}

	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End   = WeaponMesh->GetSocketLocation(TraceEndSocketName);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start,
		End,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);
}
