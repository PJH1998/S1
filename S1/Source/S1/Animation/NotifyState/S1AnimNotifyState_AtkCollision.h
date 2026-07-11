// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1AnimNotifyState_AtkCollision.generated.h"

class FPrimitiveDrawInterface;

// 데이터 마커 — 직접 히트 콜리전을 켜지 않음.
// GA_Attack이 몽타주의 이 노티파이를 스캔(GetTriggerTime/GetDuration + 아래 값)해서
// 서버에서 히트 윈도우를 1회씩 타이머로 구동 (비렌더 서버의 노티파이 per-frame thrash 우회).
UCLASS()
class S1_API US1AnimNotifyState_AtkCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	float          GetAtkScale()       const { return AtkScale; }
	FGameplayTag   GetHitStrengthTag() const { return HitStrengthTag; }
	ES1AttackHand  GetAttackHand()     const { return AttackHand; }

	// false면 이 히트 윈도우에서 무기 AttackBox를 켜지 않음 — 무기를 안 휘두르는 액션(맨손 타격 등)에서 VirtualShape만 판정하고 싶을 때 사용
	bool IsWeaponCollisionEnabled() const { return bEnableWeaponCollision; }

	// 가상 충돌체 — 무기 콜리전과 별개로, 같은 AtkScale/HitStrengthTag를 공유하는 추가 판정 도형(선택)
	ES1AtkCollisionShape  GetVirtualShape()        const { return VirtualShape; }
	ES1EffectAttachTarget GetVirtualAttachTarget() const { return VirtualAttachTarget; }
	FName                 GetVirtualSocketName()   const { return VirtualSocketName; }
	const FTransform&     GetVirtualSpawnOffset()  const { return VirtualSpawnOffset; }
	float                 GetVirtualSphereRadius() const { return VirtualSphereRadius; }
	const FVector&        GetVirtualBoxExtent()    const { return VirtualBoxExtent; }

#if WITH_EDITOR
	// 몽타주 에디터 프리뷰 뷰포트에 VirtualShape가 실제로 어디에·어떤 모양으로 찍히는지 와이어프레임으로 시각화
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

private:
	// 이 히트 윈도우의 공격 배율 — AttribSet.BaseDamage * AtkScale * Progression.GetDamageMultiplier()
	UPROPERTY(EditAnywhere, Category = "Collision")
	float AtkScale = 1.0f;

	// 피격 강도 태그 — 타겟 피격 GA 선택에 사용 (e.g. HitType.Weak / HitType.Strong)
	UPROPERTY(EditAnywhere, Category = "Collision")
	FGameplayTag HitStrengthTag;

	// 활성화할 무기 손 — Main: 단일/주무기, Offhand: 보조 무기 (기본 Main)
	UPROPERTY(EditAnywhere, Category = "Collision")
	ES1AttackHand AttackHand = ES1AttackHand::Main;

	// false면 무기 AttackBox를 켜지 않음 — 기본 true(기존 몽타주와 100% 동일)
	UPROPERTY(EditAnywhere, Category = "Collision")
	bool bEnableWeaponCollision = true;

	// None이면 가상 충돌체 비활성 — 무기 AttackBox만 동작(기존 몽타주와 100% 동일)
	UPROPERTY(EditAnywhere, Category = "VirtualCollision")
	ES1AtkCollisionShape VirtualShape = ES1AtkCollisionShape::None;

	// 가상 충돌체를 붙일 메시 — 캐릭터 본체 / 주무기 / 보조무기 소켓 중 선택
	UPROPERTY(EditAnywhere, Category = "VirtualCollision", meta = (EditCondition = "VirtualShape != ES1AtkCollisionShape::None"))
	ES1EffectAttachTarget VirtualAttachTarget = ES1EffectAttachTarget::Character;

	UPROPERTY(EditAnywhere, Category = "VirtualCollision", meta = (EditCondition = "VirtualShape != ES1AtkCollisionShape::None"))
	FName VirtualSocketName;

	// 소켓 기준 로컬 오프셋(Location/Rotation/Scale)
	UPROPERTY(EditAnywhere, Category = "VirtualCollision", meta = (EditCondition = "VirtualShape != ES1AtkCollisionShape::None"))
	FTransform VirtualSpawnOffset = FTransform::Identity;

	// Sphere/Hemisphere 반경
	UPROPERTY(EditAnywhere, Category = "VirtualCollision", meta = (EditCondition = "VirtualShape == ES1AtkCollisionShape::Sphere || VirtualShape == ES1AtkCollisionShape::Hemisphere"))
	float VirtualSphereRadius = 50.f;

	// Box 반경(Half Extent)
	UPROPERTY(EditAnywhere, Category = "VirtualCollision", meta = (EditCondition = "VirtualShape == ES1AtkCollisionShape::Box"))
	FVector VirtualBoxExtent = FVector(50.f);
};
