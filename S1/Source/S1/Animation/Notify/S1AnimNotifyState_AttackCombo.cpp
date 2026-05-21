// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/S1AnimNotifyState_AttackCombo.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "S1GameplayTags.h"

static void SetCanNextAttackTag(USkeletalMeshComponent* MeshComp, bool bAdd)
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
		ASC->AddLooseGameplayTag(S1StateTags::State_CanNextAttack);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(S1StateTags::State_CanNextAttack);
	}
}

void US1AnimNotifyState_AttackCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	SetCanNextAttackTag(MeshComp, true);
}

void US1AnimNotifyState_AttackCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	SetCanNextAttackTag(MeshComp, false);
}
