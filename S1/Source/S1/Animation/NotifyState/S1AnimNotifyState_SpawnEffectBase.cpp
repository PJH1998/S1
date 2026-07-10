// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffectBase.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Tags/S1GameplayTags.h"

US1AnimNotifyState_SpawnEffectBase::US1AnimNotifyState_SpawnEffectBase()
{
	AssetTag = S1AssetTags::Asset_Effect;
}

AS1NiagaraEffect* US1AnimNotifyState_SpawnEffectBase::FindNiagaraEffectCDO() const
{
	return S1EffectSpawnLibrary::FindNiagaraEffectCDO(AssetTag, EffectTag);
}

USkeletalMeshComponent* US1AnimNotifyState_SpawnEffectBase::ResolveTargetMesh(AS1Character* Character, USkeletalMeshComponent* CharacterMesh) const
{
	return S1EffectSpawnLibrary::ResolveTargetMesh(AttachTarget, Character, CharacterMesh);
}
