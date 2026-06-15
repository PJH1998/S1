// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "S1LogChannels.h"

AS1Weapon::AS1Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	AttackBox = CreateDefaultSubobject<UBoxComponent>("AttackBox");
	AttackBox->SetupAttachment(WeaponMesh);
	AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailComponent");
	TrailComponent->SetupAttachment(WeaponMesh, TipSocketName);
	TrailComponent->bAutoActivate = false;
}

void AS1Weapon::EnableHitCollision(float InAtkScale, FGameplayTag InHitStrengthTag)
{
	if (IsValid(AttackBox))
	{
		CurrentAtkScale       = InAtkScale;
		CurrentHitStrengthTag = InHitStrengthTag;
		OnHitCollisionEnabled.Broadcast();
		AttackBox->SetGenerateOverlapEvents(true);
		AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		LOG(TEXT("[HitCollision] Enabled | AtkScale: %.2f | Tag: %s"),
			InAtkScale, *InHitStrengthTag.ToString());
	}
}

void AS1Weapon::DisableHitCollision()
{
	if (IsValid(AttackBox))
	{
		AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AS1Weapon::EnableTrail()
{
	TrailComponent->Activate(true);
}

void AS1Weapon::DisableTrail()
{
	TrailComponent->Deactivate();
}
