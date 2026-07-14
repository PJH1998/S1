// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1ANS_AttackCollision_M.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class S1_API US1ANS_AttackCollision_M : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "AttackCollision")
	FGameplayTag CollisionTag;

	UPROPERTY(EditAnywhere, Category = "AttackCollision")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, Category = "AttackCollision")
	float DamageRatio = 1.f;

	// 피격 강도 태그 — 타겟 피격 GA 선택에 사용 (e.g. HitType.Weak / HitType.Strong)
	UPROPERTY(EditAnywhere, Category = "AttackCollision")
	FGameplayTag HitStrengthTag;

	// 무기별 사운드 카테고리 — HitStrengthTag와 조합해 US1SoundManager::PlayHitSound가 재생할 태그를 만든다 (e.g. Sound.Hit.RPR)
	UPROPERTY(EditAnywhere, Category = "AttackCollision")
	FGameplayTag SoundBaseTag;
};
