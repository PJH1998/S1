// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffectBase.h"
#include "System/S1AssetManager.h"
#include "Data/S1EffectData.h"
#include "Effect/S1Effect.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "Character/Player/S1Player.h"
#include "Weapon/S1Weapon.h"
#include "Tags/S1GameplayTags.h"

US1AnimNotifyState_SpawnEffectBase::US1AnimNotifyState_SpawnEffectBase()
{
	AssetTag = S1AssetTags::Asset_Effect;
}

AS1NiagaraEffect* US1AnimNotifyState_SpawnEffectBase::FindNiagaraEffectCDO() const
{
	US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(AssetTag);
	if (nullptr == EffectData)
	{
		return nullptr;
	}

	TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(EffectTag);
	if (nullptr == EffectClass)
	{
		return nullptr;
	}

	return Cast<AS1NiagaraEffect>(EffectClass->GetDefaultObject<AS1Effect>());
}

USkeletalMeshComponent* US1AnimNotifyState_SpawnEffectBase::ResolveTargetMesh(AS1Character* Character, USkeletalMeshComponent* CharacterMesh) const
{
	if (ES1EffectAttachTarget::Character == AttachTarget)
	{
		return CharacterMesh;
	}

	AS1Player* Player = Cast<AS1Player>(Character);
	AS1Weapon* Weapon = ::IsValid(Player) ? Player->GetEquippedWeapon() : nullptr;
	return ::IsValid(Weapon) ? Weapon->GetWeaponMesh() : nullptr;
}
