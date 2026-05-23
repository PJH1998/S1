// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/LooseGameplayTag/S1AnimNotifyState_LooseGameplayTag.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void US1AnimNotifyState_LooseGameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SetTag(MeshComp, true);
}

void US1AnimNotifyState_LooseGameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	SetTag(MeshComp, false);
}

void US1AnimNotifyState_LooseGameplayTag::SetTag(USkeletalMeshComponent* MeshComp, bool bAdd)
{
	if (false == Tag.IsValid())
	{
		return;
	}

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
		ASC->AddLooseGameplayTag(Tag);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}
