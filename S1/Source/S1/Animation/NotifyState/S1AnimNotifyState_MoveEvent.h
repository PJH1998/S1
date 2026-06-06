// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotifyState_MoveEvent.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_MoveEvent : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	FGameplayTag MoveBeginEventTag;

	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	FGameplayTag MoveEndEventTag;
};
