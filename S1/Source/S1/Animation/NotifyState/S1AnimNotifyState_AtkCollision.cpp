// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_AtkCollision.h"
#include "Character/Player/S1Player.h"
#include "Weapon/S1Weapon.h"

void US1AnimNotifyState_AtkCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AS1Player* Player = Cast<AS1Player>(MeshComp->GetOwner());
	if (false == IsValid(Player))
	{
		return;
	}

	AS1Weapon* Weapon = Player->GetEquippedWeapon();
	if (false == IsValid(Weapon))
	{
		return;
	}

	Weapon->EnableHitCollision(AtkScale, HitStrengthTag);
}

void US1AnimNotifyState_AtkCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AS1Player* Player = Cast<AS1Player>(MeshComp->GetOwner());
	if (false == IsValid(Player))
	{
		return;
	}

	AS1Weapon* Weapon = Player->GetEquippedWeapon();
	if (false == IsValid(Weapon))
	{
		return;
	}

	Weapon->DisableHitCollision();
}
