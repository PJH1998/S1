// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Boss000/Notify/S1AnimNotify_CheckDistance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"

void US1AnimNotify_CheckDistance::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr || Animation == nullptr)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = Owner;
	Payload.Target = Owner;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}
