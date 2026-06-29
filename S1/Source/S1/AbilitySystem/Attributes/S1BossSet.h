// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "S1BossSet.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1BossSet : public US1MonsterSet
{
	GENERATED_BODY()

public:
	// 가드 중 정면 데미지 차단 + HP 50% 미만 시 페이즈 전환 요청.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
