// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1AnimNotify_RequestInertialization.generated.h"

// 몽타주 섹션 전환 시 부드러운 보간이 필요한 지점(e.g. Start→Loop)에 배치
// 자동으로 RequestInertialization을 호출 — AnimGraph에 Inertialization 노드 필요
UCLASS()
class S1_API US1AnimNotify_RequestInertialization : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Inertialization", meta = (ClampMin = "0.0"))
	float BlendTime = 0.2f;
};
