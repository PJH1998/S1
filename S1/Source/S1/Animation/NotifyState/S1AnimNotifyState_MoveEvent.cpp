// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_MoveEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"

void US1AnimNotifyState_MoveEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (false == MoveBeginEventTag.IsValid())
	{
		return;
	}

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (false == IsValid(Owner))
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventMagnitude = ForwardImpulse;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, MoveBeginEventTag, Payload);
}

void US1AnimNotifyState_MoveEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (false == MoveEndEventTag.IsValid())
	{
		return;
	}

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (false == IsValid(Owner))
	{
		return;
	}

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, MoveEndEventTag, Payload);
}
