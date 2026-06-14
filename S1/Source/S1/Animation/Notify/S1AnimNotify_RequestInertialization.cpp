// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_RequestInertialization.h"
#include "Animation/S1AnimInstance.h"

void US1AnimNotify_RequestInertialization::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}

	US1AnimInstance* AnimInst = Cast<US1AnimInstance>(MeshComp->GetAnimInstance());
	if (false == IsValid(AnimInst))
	{
		return;
	}

	AnimInst->RequestInertialization(BlendTime);
}
