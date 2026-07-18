// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_DespawnHealItem.h"
#include "Character/Player/S1Player.h"

void US1AnimNotify_DespawnHealItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	Player->DespawnHealItem(DissolveDuration);
}
