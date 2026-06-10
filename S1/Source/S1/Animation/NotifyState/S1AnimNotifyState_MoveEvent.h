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

	// 이동 속도 — Payload.EventMagnitude로 전달, GA의 OnMoveBeginReceived에서 사용
	// 0이면 이동 없음
	UPROPERTY(EditAnywhere, Category = "MoveEvent")
	float ForwardImpulse = 0.f;
};
