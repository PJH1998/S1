// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_RequestInertialization.h"
#include "Animation/S1AnimInstance.h"
#include "S1LogChannels.h"

void US1AnimNotify_RequestInertialization::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == IsValid(MeshComp))
	{
		LOG(TEXT("[Inertialization] Notify — MeshComp invalid"));
		return;
	}

	US1AnimInstance* AnimInst = Cast<US1AnimInstance>(MeshComp->GetAnimInstance());
	if (false == IsValid(AnimInst))
	{
		LOG(TEXT("[Inertialization] Notify — AnimInstance가 US1AnimInstance 캐스트 실패: %s"),
			MeshComp->GetAnimInstance() ? *MeshComp->GetAnimInstance()->GetClass()->GetName() : TEXT("None"));
		return;
	}

	LOG(TEXT("[Inertialization] Notify 발화 — BlendTime: %f"), BlendTime);
	AnimInst->RequestInertialization(BlendTime);
}
