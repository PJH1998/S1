// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1AnimNotifyState_Chase.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1AnimNotifyState_Chase : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category="Rotate")
	float RotationSpeed = { 180.f };
};
