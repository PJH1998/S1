// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1Enums.h"
#include "S1AnimNotify_SpawnHealItem.generated.h"

class AS1HealItem;
class FPrimitiveDrawInterface;

// 힐 아이템 소품(AS1HealItem)을 소켓+오프셋 위치에 부착하고 Dissolve로 나타냄 — 라이프타임 무제한(US1AnimNotify_DespawnHealItem으로 명시적 제거)
UCLASS()
class S1_API US1AnimNotify_SpawnHealItem : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	// 몽타주 에디터 프리뷰 뷰포트에 스폰 위치를 축+구 마커로 시각화 (실제 스폰은 안 함 — 스크러빙 중 반복 호출로 액터가 쌓이는 문제 방지)
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

private:
	UPROPERTY(EditAnywhere, Category = "HealItem")
	TSubclassOf<AS1HealItem> HealItemClass;

	UPROPERTY(EditAnywhere, Category = "HealItem")
	ES1AttackHand Hand = ES1AttackHand::Offhand;

	// 소켓 기준 로컬 오프셋(위치/회전/크기) — AS1HealItem의 ItemMesh가 회전은 SetAbsolute로 소켓과 디커플링돼 있어
	// 여기서 지정한 Rotation이 곧 월드 회전으로 고정됨(손 애니메이션에 영향받지 않음)
	UPROPERTY(EditAnywhere, Category = "HealItem")
	FTransform AttachOffset = FTransform::Identity;

	// 초당 회전량 (Yaw 90 = 1.33초에 한 바퀴)
	UPROPERTY(EditAnywhere, Category = "HealItem")
	FRotator RotationRate = FRotator(0.f, 90.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "HealItem", meta = (ClampMin = "0.0"))
	float DissolveDuration = 0.3f;
};
