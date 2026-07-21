// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_WeaponDissolve.h"
#include "Character/Player/S1Player.h"
#include "Weapon/S1Weapon.h"

void US1AnimNotify_WeaponDissolve::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	AS1Player* Player = Cast<AS1Player>(MeshComp->GetOwner());
	if (nullptr == Player)
	{
		return;
	}

	AS1Weapon* Weapon = (Hand == ES1AttackHand::Offhand) ? Player->GetEquippedOffhandWeapon() : Player->GetEquippedWeapon();
	if (IsValid(Weapon))
	{
		Weapon->PlayDissolve(bAppear, Duration);
	}
}
