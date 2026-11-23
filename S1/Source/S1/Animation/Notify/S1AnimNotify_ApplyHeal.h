// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1AnimNotify_ApplyHeal.generated.h"

class UGameplayEffect;

// CurrentHealth를 MaxHealth 비율만큼 즉시 회복 — 서버(HasAuthority)에서만 회복 GE를 적용,
// 나머지 머신(몽타주 재생용)에서는 아무 것도 하지 않음
UCLASS()
class S1_API US1AnimNotify_ApplyHeal : public UAnimNotify
{
	GENERATED_BODY()

public:
	US1AnimNotify_ApplyHeal();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// MaxHealth 대비 회복 비율 (0.4 = 40%)
	UPROPERTY(EditAnywhere, Category = "Heal", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealPercentOfMaxHealth = 0.4f;

	// 회복 GE — 회복량은 SetByCaller(SetByCaller.Heal)로 넘긴다. 기본값은 US1GameplayEffect_Heal
	UPROPERTY(EditAnywhere, Category = "Heal")
	TSubclassOf<UGameplayEffect> HealEffectClass;
};
