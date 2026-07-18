// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_RateScale.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

void US1AnimNotifyState_RateScale::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UAnimInstance* AnimInstance = ::IsValid(MeshComp) ? MeshComp->GetAnimInstance() : nullptr;
	if (false == ::IsValid(AnimInstance))
	{
		return;
	}

	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	if (false == ::IsValid(Montage))
	{
		return;
	}

	AnimInstance->Montage_SetPlayRate(Montage, RateScale);
}

void US1AnimNotifyState_RateScale::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UAnimInstance* AnimInstance = ::IsValid(MeshComp) ? MeshComp->GetAnimInstance() : nullptr;
	if (false == ::IsValid(AnimInstance))
	{
		return;
	}

	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	if (false == ::IsValid(Montage))
	{
		return;
	}

	AnimInstance->Montage_SetPlayRate(Montage, 1.0f);
}
