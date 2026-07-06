// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffect.h"
#include "S1AnimNotifyState_SpawnEffectWithProgress.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_SpawnEffectWithProgress : public US1AnimNotifyState_SpawnEffect
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
	// Niagara System에 노출된 Float User Parameter 이름 — 이 Notify의 Begin~End 진행률(0~1)이 매 프레임 여기로 전달됨
	UPROPERTY(EditAnywhere, Category = "Effect")
	FName ProgressParameterName = TEXT("RevealProgress");
};
