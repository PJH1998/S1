// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_PlayBGMByTag.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "System/S1SoundManager.h"

void US1AnimNotify_PlayBGMByTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == ::IsValid(MeshComp))
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	if (US1SoundManager* SoundManager = World->GetSubsystem<US1SoundManager>())
	{
		SoundManager->PlayBGMByTag(BGMTag, FadeDuration, Volume);
	}
}
