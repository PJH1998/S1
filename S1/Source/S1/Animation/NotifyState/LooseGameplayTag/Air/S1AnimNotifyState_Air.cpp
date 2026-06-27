// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/LooseGameplayTag/Air/S1AnimNotifyState_Air.h"
#include "Animation/Player/S1PlayerAnimInstance.h"
#include "GameFramework/Character.h"

void US1AnimNotifyState_Air::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// Super(SetTag) 미호출 — State.Air는 AS1Player::OnMovementModeChanged(CMC Falling 기준)가 단일 관리
	// (노티파이가 태그를 추가하고 제거 안 하면 OnMovementModeChanged와 충돌해 태그가 안 풀림)
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
