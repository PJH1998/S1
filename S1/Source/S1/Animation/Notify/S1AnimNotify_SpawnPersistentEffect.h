// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1AnimNotify_SpawnPersistentEffect.generated.h"

class UNiagaraSystem;
class FPrimitiveDrawInterface;

// 몽타주 Loop/Branch처럼 NotifyState의 End가 신뢰성 있게 호출되지 않는 경우를 위한 단발 스폰 Notify
// — US1AnimNotify_DespawnEffect와 EffectTag로 짝을 이뤄 사용. NS 자체는 Loop Behavior=Infinite 등으로
// 스스로 끝나지 않게 세팅되어 있어야 함(코드가 지속시간을 강제 연장하지 않음)
UCLASS()
class S1_API US1AnimNotify_SpawnPersistentEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	US1AnimNotify_SpawnPersistentEffect();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	// 몽타주 에디터 프리뷰 뷰포트에 SpawnOffset이 실제로 어디에 찍히는지 축+구 마커로 시각화(스크러빙 중인 현재 프레임 기준)
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag AssetTag;

	// EffectData에서 VFX를 찾는 태그이자, Character::ActiveAttachedEffects에 등록되는 Key(Despawn에서 같은 값으로 찾음)
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag EffectTag;

	// Weapon이면 장착 무기의 WeaponMesh 소켓에서, Character면 캐릭터 메시 소켓에서 스폰
	UPROPERTY(EditAnywhere, Category = "Effect")
	ES1EffectAttachTarget AttachTarget = ES1EffectAttachTarget::Weapon;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName SpawnSocketName;

	// 소켓 기준 로컬 오프셋(Location/Rotation/Scale)
	UPROPERTY(EditAnywhere, Category = "Effect")
	FTransform SpawnOffset = FTransform::Identity;

	// true면 소켓에 계속 붙어서 따라다님(Despawn에서 Detach), false면 스폰 시점 위치/회전에 스냅샷으로 고정
	UPROPERTY(EditAnywhere, Category = "Effect")
	bool bAttachToSocket = false;

	// 스폰 시 NS의 Color User Parameter에 1회 설정 — 캐릭터/스킬별로 같은 NS를 다른 색으로 재사용할 때 사용
	UPROPERTY(EditAnywhere, Category = "Effect")
	FLinearColor InitialColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName ColorParameterName = TEXT("Color");

#if WITH_EDITORONLY_DATA
	// 애니메이션/몽타주 에디터 프리뷰 전용 확인용 — 실제 게임플레이·PIE에서는 절대 사용 안 됨
	UPROPERTY(EditAnywhere, Category = "Effect|Preview")
	TObjectPtr<UNiagaraSystem> PreviewNiagaraSystem;
#endif
};
