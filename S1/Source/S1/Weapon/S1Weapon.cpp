// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Component/S1DissolveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

AS1Weapon::AS1Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// 서버에서 스폰·부착 → 모든 클라에 복제 (부착 정보 포함)
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	AttackBox = CreateDefaultSubobject<UBoxComponent>("AttackBox");
	AttackBox->SetupAttachment(WeaponMesh);
	AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailComponent");
	TrailComponent->SetupAttachment(WeaponMesh, TipSocketName);
	TrailComponent->bAutoActivate = false;

	DissolveComponent = CreateDefaultSubobject<US1DissolveComponent>("DissolveComponent");
}

void AS1Weapon::PlayDissolve(bool bAppear, float Duration)
{
	if (IsValid(DissolveComponent) && IsValid(WeaponMesh))
	{
		DissolveComponent->PlayDissolve(WeaponMesh, bAppear, Duration);
	}
}

void AS1Weapon::EnableHitCollision(float InAtkScale, FGameplayTag InHitStrengthTag, FGameplayTag InSoundBaseTag)
{
	if (IsValid(AttackBox))
	{
		CurrentAtkScale       = InAtkScale;
		CurrentHitStrengthTag = InHitStrengthTag;
		CurrentSoundBaseTag   = InSoundBaseTag;
		OnHitCollisionEnabled.Broadcast();
		AttackBox->SetGenerateOverlapEvents(true);
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

void AS1Weapon::EnableTrail()
{
	TrailComponent->Activate(true);
}

void AS1Weapon::DisableTrail()
{
	TrailComponent->Deactivate();
}
