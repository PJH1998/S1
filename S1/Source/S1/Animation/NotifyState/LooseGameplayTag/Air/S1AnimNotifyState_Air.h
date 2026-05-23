// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/NotifyState/LooseGameplayTag/S1AnimNotifyState_LooseGameplayTag.h"
#include "S1AnimNotifyState_Air.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_Air : public US1AnimNotifyState_LooseGameplayTag
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Air")
	float AdditionalLaunchZ = 500.f;
};
