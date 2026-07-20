// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_GrantLooseGameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void US1AnimNotify_GrantLooseGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp || false == TagToGrant.IsValid())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (nullptr == Owner || false == Owner->HasAuthority())
	{
		return;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (nullptr == ASC)
	{
		return;
	}

	ASC->AddLooseGameplayTag(TagToGrant, 1, EGameplayTagReplicationState::TagOnly);
}
