// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/S1Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Animation/Weapon/S1WeaponAnimLayer.h"

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
}

TSubclassOf<US1WeaponAnimLayer> AS1Weapon::GetAnimLayerClass(EPlayerGender InGender) const
{
	return (InGender == EPlayerGender::Female) ? FemaleAnimLayerClass : MaleAnimLayerClass;
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
