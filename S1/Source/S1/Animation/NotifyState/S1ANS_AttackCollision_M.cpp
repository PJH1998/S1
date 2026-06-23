// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/S1ANS_AttackCollision_M.h"

#include "Character/S1Monster.h"
#include "Components/SkeletalMeshComponent.h"

void US1ANS_AttackCollision_M::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(MeshComp->GetOwner()))
	{
		if (false == Monster->HasAuthority())
		{
			return;
		}

		Monster->EnableAttackCollision(CollisionTag, DamageEffect, DamageRatio);
	}
}

void US1ANS_AttackCollision_M::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(MeshComp->GetOwner()))
	{
		if (false == Monster->HasAuthority())
		{
			return;
		}

		Monster->DisableAttackCollision(CollisionTag);
	}
}
