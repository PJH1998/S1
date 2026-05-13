// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/S1AnimNotifyState_WeakCombo.h"
#include "Animation/S1PlayerAnimInstance.h"

void US1AnimNotifyState_WeakCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	US1PlayerAnimInstance* AnimInst = Cast<US1PlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (nullptr == AnimInst)
	{
		return;
	}

	AnimInst->bCanNextAttack = true;
}

void US1AnimNotifyState_WeakCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	US1PlayerAnimInstance* AnimInst = Cast<US1PlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (nullptr == AnimInst)
	{
		return;
	}

	AnimInst->bCanNextAttack = false;
}

