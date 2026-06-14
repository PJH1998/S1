// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_RootMotionScale.h"
#include "GameFramework/Character.h"

void US1AnimNotifyState_RootMotionScale::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (false == IsValid(Character))
	{
		return;
	}

	Character->SetAnimRootMotionTranslationScale(Scale);
}

void US1AnimNotifyState_RootMotionScale::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (false == IsValid(Character))
	{
		return;
	}

	Character->SetAnimRootMotionTranslationScale(1.0f);
}
