// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_Air.h"
#include "Animation/Player/S1PlayerAnimInstance.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "S1GameplayTags.h"

static void SetAirTag(USkeletalMeshComponent* MeshComp, bool bAdd)
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (nullptr == ASI)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (nullptr == ASC)
	{
		return;
	}

	if (bAdd)
	{
		ASC->AddLooseGameplayTag(S1StateTags::State_Air);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Air);
	}
}

void US1AnimNotifyState_Air::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	SetAirTag(MeshComp, true);

	// Root Motion 중 GravityScale은 효과 없음 —> LaunchCharacter로 추가 높이 부여
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->LaunchCharacter(FVector(0.f, 0.f, AdditionalLaunchZ), false, true);
	}
}

void US1AnimNotifyState_Air::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (US1PlayerAnimInstance* AnimInst = Cast<US1PlayerAnimInstance>(MeshComp->GetAnimInstance()))
	{
		AnimInst->SetDirectToFall(true);
	}
}
