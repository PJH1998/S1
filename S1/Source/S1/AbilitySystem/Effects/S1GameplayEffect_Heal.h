// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "S1GameplayEffect_Heal.generated.h"

// 체력 회복 GE — Instant / Health Additive / SetByCaller(SetByCaller.Heal)
// 회복량은 적용하는 쪽이 SetByCaller로 넘기고, MaxHealth 초과분은 S1AttributeSet::PostGameplayEffectExecute가 클램프한다.
// C++ 클래스로 둔 이유: 회복 노티파이가 여러 몽타주에 박혀 있어 BP 애셋이면 노티파이마다 지정해야 한다 (BP 자식으로 교체는 가능)
UCLASS()
class S1_API US1GameplayEffect_Heal : public UGameplayEffect
{
	GENERATED_BODY()

public:
	US1GameplayEffect_Heal();
};
