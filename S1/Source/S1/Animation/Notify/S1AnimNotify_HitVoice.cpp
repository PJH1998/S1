// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_HitVoice.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Player/S1Player.h"

void US1AnimNotify_HitVoice::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == ::IsValid(MeshComp))
	{
		return;
	}

	AS1Player* Player = Cast<AS1Player>(MeshComp->GetOwner());
	if (false == ::IsValid(Player))
	{
		return;
	}

	USoundBase* VoiceSound = (EPlayerGender::Female == Player->GetGender()) ? FemaleSound : MaleSound;
	if (nullptr == VoiceSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(Player, VoiceSound, Player->GetActorLocation());
}
