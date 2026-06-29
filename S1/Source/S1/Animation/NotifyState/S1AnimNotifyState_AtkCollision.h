// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotifyState_AtkCollision.generated.h"

// 데이터 마커 — 직접 히트 콜리전을 켜지 않음.
// GA_Attack이 몽타주의 이 노티파이를 스캔(GetTriggerTime/GetDuration + 아래 값)해서
// 서버에서 히트 윈도우를 1회씩 타이머로 구동 (비렌더 서버의 노티파이 per-frame thrash 우회).
UCLASS()
class S1_API US1AnimNotifyState_AtkCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	float        GetAtkScale()       const { return AtkScale; }
	FGameplayTag GetHitStrengthTag() const { return HitStrengthTag; }

private:
	// 이 히트 윈도우의 공격 배율 — AttribSet.BaseDamage * AtkScale * Progression.GetDamageMultiplier()
	UPROPERTY(EditAnywhere, Category = "Collision")
	float AtkScale = 1.0f;

	// 피격 강도 태그 — 타겟 피격 GA 선택에 사용 (e.g. HitType.Weak / HitType.Strong)
	UPROPERTY(EditAnywhere, Category = "Collision")
	FGameplayTag HitStrengthTag;
};
