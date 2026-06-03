// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_SpawnEffect.h"
#include "System/S1AssetManager.h"
#include "Data/S1EffectData.h"
#include "Effect/S1Effect.h"
#include "S1GameplayTags.h"

US1AnimNotify_SpawnEffect::US1AnimNotify_SpawnEffect()
{
	AssetTag = S1AssetTags::Asset_Effect;
}

void US1AnimNotify_SpawnEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (nullptr == Owner)
	{
		return;
	}

	US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(AssetTag);
	if (nullptr == EffectData)
	{
		return;
	}

	TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(EffectTag);
	if (nullptr == EffectClass)
	{
		return;
	}

	FVector Location = MeshComp->GetSocketLocation(SpawnSocketName);
	FRotator Rotation = MeshComp->GetSocketRotation(SpawnSocketName);

	EffectClass->GetDefaultObject<AS1Effect>()->PlayEffect(Owner->GetWorld(), Location, Rotation);
}
