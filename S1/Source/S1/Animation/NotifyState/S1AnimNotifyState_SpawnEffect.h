// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffectBase.h"
#include "S1AnimNotifyState_SpawnEffect.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_SpawnEffect : public US1AnimNotifyState_SpawnEffectBase
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	bool IsAttachToSocket() const { return bAttachToSocket; }

private:
	// true면 소켓에 계속 붙어서 따라다니다 End에 Detach, false면 스폰 시점 위치/회전에 스냅샷으로 고정
	UPROPERTY(EditAnywhere, Category = "Effect")
	bool bAttachToSocket = false;

	// bAttachToSocket=true일 때만 의미 있음 — false면 스폰 시점 회전으로 고정(소켓 회전을 안 따라감). 횡베기처럼 회전을 그대로 먹으면 부자연스러운 경우 끄기
	UPROPERTY(EditAnywhere, Category = "Effect", meta = (EditCondition = "bAttachToSocket"))
	bool bFollowSocketRotation = true;
};
