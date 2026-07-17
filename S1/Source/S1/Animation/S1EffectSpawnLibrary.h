// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"

class AS1Character;
class AS1NiagaraEffect;

// UAnimNotify / UAnimNotifyState는 엔진에서 서로 형제 클래스라 상속으로 로직을 공유할 수 없음 —
// 이펙트 스폰에 필요한 공용 로직을 여기 static 함수로 모아서 양쪽 다 갖다 씀(중복 정의 방지)
namespace S1EffectSpawnLibrary
{
	// AssetTag/EffectTag로 EffectData에서 Niagara Effect CDO를 찾음 — 못 찾으면 nullptr
	AS1NiagaraEffect* FindNiagaraEffectCDO(const FGameplayTag& AssetTag, const FGameplayTag& EffectTag);

	// AttachTarget 설정에 따라 Weapon(장착 무기 메시)/Character(캐릭터 메시) 중 스폰 대상 메시 결정 — 못 찾으면 nullptr
	USkeletalMeshComponent* ResolveTargetMesh(ES1EffectAttachTarget AttachTarget, AS1Character* Character, USkeletalMeshComponent* CharacterMesh);

	// 소켓의 월드 트랜스폼에 SpawnOffset을 합성 — 본 스윙 영향 없이 쓰고 싶으면 회전 정보가 없는(고정) 본에 소켓을 잡아서 사용
	FTransform ComputeSpawnTransform(USkeletalMeshComponent* TargetMesh, FName SocketName, const FTransform& SpawnOffset);
}
