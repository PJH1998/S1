// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffectBase.h"
#include "S1AnimNotifyState_SpawnEffect.generated.h"

class FPrimitiveDrawInterface;

UCLASS()
class S1_API US1AnimNotifyState_SpawnEffect : public US1AnimNotifyState_SpawnEffectBase
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	// 몽타주 에디터 프리뷰 뷰포트에 SpawnOffset이 실제로 어디에 찍히는지 축+구 마커로 시각화(스크러빙 중인 현재 프레임 기준)
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

protected:
	bool IsAttachToSocket() const { return bAttachToSocket; }

private:
	// true면 소켓에 계속 붙어서 따라다니다 End에 Detach, false면 스폰 시점 위치/회전에 스냅샷으로 고정
	UPROPERTY(EditAnywhere, Category = "Effect")
	bool bAttachToSocket = false;
};
