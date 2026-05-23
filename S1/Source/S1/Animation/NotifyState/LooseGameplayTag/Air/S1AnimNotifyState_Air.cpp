// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/LooseGameplayTag/Air/S1AnimNotifyState_Air.h"
#include "Animation/Player/S1PlayerAnimInstance.h"
#include "GameFramework/Character.h"

void US1AnimNotifyState_Air::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);	// SetTag(true) — Tag는 에디터에서 State.Air로 설정

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->LaunchCharacter(FVector(0.f, 0.f, AdditionalLaunchZ), false, true);
	}
}

void US1AnimNotifyState_Air::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Super 미호출 — State.Air 태그는 Landed()에서 별도 제거
	if (US1PlayerAnimInstance* AnimInst = Cast<US1PlayerAnimInstance>(MeshComp->GetAnimInstance()))
	{
		AnimInst->SetDirectToFall(true);
	}
}
