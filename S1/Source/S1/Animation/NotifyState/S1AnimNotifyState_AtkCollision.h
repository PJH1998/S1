// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotifyState_AtkCollision.generated.h"

UCLASS()
class S1_API US1AnimNotifyState_AtkCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// 이 히트 윈도우의 공격 배율 — AttribSet.BaseDamage * AtkScale * Progression.GetDamageMultiplier()
	UPROPERTY(EditAnywhere, Category = "Collision")
	float AtkScale = 1.0f;

	// 피격 강도 태그 — 타겟 피격 GA 선택에 사용 (e.g. HitType.Weak / HitType.Strong)
	UPROPERTY(EditAnywhere, Category = "Collision")
	FGameplayTag HitStrengthTag;
};
