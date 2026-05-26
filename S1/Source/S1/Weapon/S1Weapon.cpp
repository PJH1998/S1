// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

AS1Weapon::AS1Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	AttackBox = CreateDefaultSubobject<UBoxComponent>("AttackBox");
	AttackBox->SetupAttachment(WeaponMesh);
	AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AS1Weapon::EnableHitCollision()
{
	if (IsValid(AttackBox))
	{
		OnHitCollisionEnabled.Broadcast();
		AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AS1Weapon::DisableHitCollision()
{
	if (IsValid(AttackBox))
	{
		AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
