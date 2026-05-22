// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_AttackCombo.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void US1AnimNotifyState_AttackCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (false == CanNextAttackTag.IsValid())
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

	ASC->AddLooseGameplayTag(CanNextAttackTag);
}

void US1AnimNotifyState_AttackCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (false == CanNextAttackTag.IsValid())
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

	ASC->RemoveLooseGameplayTag(CanNextAttackTag);
}
