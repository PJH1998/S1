// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1AnimNotify_SpawnLoopCycleEffect.generated.h"

class UNiagaraSystem;
class FPrimitiveDrawInterface;

// RepeatLoop 계열(Charge 등) LoopMontage의 사이클 지점에 배치 — US1AnimNotify_SendGameplayEvent를 대체
// (LoopCycleEventTag는 그대로 사용하므로 이 Notify를 안 쓰는 기존 Loop들은 영향 없음)
// 1) 아직 스폰 안 된 경우(첫 사이클)에만 이펙트를 스폰/등록 — 이후 사이클은 재스폰하지 않음(GA가 Activate(true)로 재트리거)
// 2) 항상 EventTag로 GameplayEvent를 보내되, Payload.InstigatorTags에 EffectTag를 실어서 GA가 어떤 이펙트인지 알 수 있게 함
UCLASS()
class S1_API US1AnimNotify_SpawnLoopCycleEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	US1AnimNotify_SpawnLoopCycleEffect();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	// 몽타주 에디터 프리뷰 뷰포트에 SpawnOffset이 실제로 어디에 찍히는지 축+구 마커로 시각화(스크러빙 중인 현재 프레임 기준)
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag AssetTag;

	// EffectData에서 VFX를 찾는 태그이자, Character::ActiveAttachedEffects에 등록되는 Key — Progression을 거쳐 GA에도 이 태그로 전달됨
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

	// true면 소켓에 계속 붙어서 따라다님, false면 스폰 시점 위치/회전에 스냅샷으로 고정
	UPROPERTY(EditAnywhere, Category = "Effect")
	bool bAttachToSocket = true;

	// 스폰 시 NS의 Color User Parameter에 1회 설정 — 캐릭터/스킬별로 같은 NS를 다른 색으로 재사용할 때 사용
	UPROPERTY(EditAnywhere, Category = "Effect")
	FLinearColor InitialColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName ColorParameterName = TEXT("Color");

	// US1MontageProgression_Loop::LoopCycleEventTag와 동일 값이어야 함
	UPROPERTY(EditAnywhere, Category = "Effect|Event")
	FGameplayTag EventTag;

#if WITH_EDITORONLY_DATA
	// 애니메이션/몽타주 에디터 프리뷰 전용 확인용 — 실제 게임플레이·PIE에서는 절대 사용 안 됨
	UPROPERTY(EditAnywhere, Category = "Effect|Preview")
	TObjectPtr<UNiagaraSystem> PreviewNiagaraSystem;
#endif
};
