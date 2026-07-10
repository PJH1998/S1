// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/S1EffectSpawnLibrary.h"
#include "System/S1AssetManager.h"
#include "Data/S1EffectData.h"
#include "Effect/S1Effect.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "Character/Player/S1Player.h"
#include "Weapon/S1Weapon.h"

AS1NiagaraEffect* S1EffectSpawnLibrary::FindNiagaraEffectCDO(const FGameplayTag& AssetTag, const FGameplayTag& EffectTag)
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

USkeletalMeshComponent* S1EffectSpawnLibrary::ResolveTargetMesh(ES1EffectAttachTarget AttachTarget, AS1Character* Character, USkeletalMeshComponent* CharacterMesh)
{
	if (ES1EffectAttachTarget::Character == AttachTarget)
	{
		return CharacterMesh;
	}

	AS1Player* Player = Cast<AS1Player>(Character);
	if (false == ::IsValid(Player))
	{
		return nullptr;
	}

	AS1Weapon* Weapon = (ES1EffectAttachTarget::OffHandWeapon == AttachTarget)
		? Player->GetEquippedOffhandWeapon()
		: Player->GetEquippedWeapon();

	return ::IsValid(Weapon) ? Weapon->GetWeaponMesh() : nullptr;
}

FTransform S1EffectSpawnLibrary::ComputeSpawnTransform(USkeletalMeshComponent* TargetMesh, FName SocketName, const FTransform& SpawnOffset)
{
	return SpawnOffset * TargetMesh->GetSocketTransform(SocketName);
}
