// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

AS1Weapon::AS1Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	HitBox = CreateDefaultSubobject<UBoxComponent>("HitBox");
	HitBox->SetupAttachment(WeaponMesh);
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AS1Weapon::EnableHitCollision()
{
	if (IsValid(HitBox))
	{
		HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AS1Weapon::DisableHitCollision()
{
	if (IsValid(HitBox))
	{
		HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
