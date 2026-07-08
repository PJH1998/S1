// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_DespawnEffect.h"
#include "Character/S1Character.h"

void US1AnimNotify_DespawnEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	AS1Character* Character = Cast<AS1Character>(MeshComp->GetOwner());
	if (nullptr == Character)
	{
		return;
	}

	Character->EndAttachedEffect(EffectTag, LifetimeAfterEnd);
}
