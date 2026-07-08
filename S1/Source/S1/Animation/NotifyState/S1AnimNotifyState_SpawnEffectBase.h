// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1AnimNotifyState_SpawnEffectBase.generated.h"

class UNiagaraSystem;
class AS1Character;
class AS1NiagaraEffect;

UCLASS(Abstract)
class S1_API US1AnimNotifyState_SpawnEffectBase : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	US1AnimNotifyState_SpawnEffectBase();

protected:
	FGameplayTag GetEffectTag() const { return EffectTag; }
	FName GetSpawnSocketName() const { return SpawnSocketName; }
	const FTransform& GetSpawnOffset() const { return SpawnOffset; }
	ES1EffectAttachTarget GetAttachTarget() const { return AttachTarget; }
	const FLinearColor& GetInitialColor() const { return InitialColor; }
	FName GetColorParameterName() const { return ColorParameterName; }

	// AssetTag/EffectTag로 EffectData에서 Niagara Effect CDO를 찾음 — 못 찾으면 nullptr
	AS1NiagaraEffect* FindNiagaraEffectCDO() const;

	// AttachTarget 설정에 따라 Weapon(장착 무기 메시)/Character(캐릭터 메시) 중 스폰 대상 메시 결정 — 못 찾으면 nullptr
	USkeletalMeshComponent* ResolveTargetMesh(AS1Character* Character, USkeletalMeshComponent* CharacterMesh) const;

#if WITH_EDITORONLY_DATA
	bool IsEditorPreviewWorld(const UWorld* World) const { return nullptr != World && EWorldType::EditorPreview == World->WorldType; }

	// 애니메이션/몽타주 에디터 프리뷰 전용 확인용 — 실제 게임플레이·PIE에서는 절대 사용 안 됨.
	// WITH_EDITORONLY_DATA=0인 Shipping/데디서버 빌드에서는 이 프로퍼티 자체가 컴파일에서 제외되어 메모리를 전혀 차지하지 않음
	UPROPERTY(EditAnywhere, Category = "Effect|Preview")
	TObjectPtr<UNiagaraSystem> PreviewNiagaraSystem;
#endif

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag AssetTag;

	// EffectData에서 VFX를 찾는 태그이자, Character::ActiveAttachedEffects에 등록되는 Key로 같이 사용됨
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag EffectTag;

	// Weapon이면 장착 무기의 WeaponMesh 소켓에서, Character면 캐릭터 메시 소켓에서 스폰
	UPROPERTY(EditAnywhere, Category = "Effect")
	ES1EffectAttachTarget AttachTarget = ES1EffectAttachTarget::Weapon;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName SpawnSocketName;

	// 소켓 기준 로컬 오프셋(Location/Rotation/Scale) — Effect Local에 곱해지는 소켓 상대 트랜스폼
	UPROPERTY(EditAnywhere, Category = "Effect")
	FTransform SpawnOffset = FTransform::Identity;

	// 스폰 시 NS의 Color User Parameter에 1회 설정 — 캐릭터/스킬별로 같은 NS를 다른 색으로 재사용할 때 사용
	// (ColorParameterName이 NS에 없는 이름이면 Niagara가 조용히 무시함 — 안 써도 안전)
	UPROPERTY(EditAnywhere, Category = "Effect")
	FLinearColor InitialColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName ColorParameterName = TEXT("Color");
};
