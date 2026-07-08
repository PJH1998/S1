// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/Air/S1GameplayAbility_Attack_Air.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Charge_Air.generated.h"

// US1GameplayAbility_Charge의 공중 버전 — 단일 상속 제약으로 US1GameplayAbility_Attack_Air(중력 처리)를 상속해야 해서
// 사이클당 이펙트 갱신 로직(US1GameplayAbility_Charge와 동일)을 여기 다시 선언 — Progression 상속만으로는 재사용 불가
UCLASS()
class S1_API US1GameplayAbility_Charge_Air : public US1GameplayAbility_Attack_Air
{
	GENERATED_BODY()

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	// RepeatLoop 계열 Progression이 사이클 완료마다 호출 — EffectTag로 컴포넌트를 찾아 Ratio·크기 갱신
	virtual void OnProgressionCycleCompleted(FGameplayTag EffectTag, int32 CycleCount, int32 MaxCycleCount) override;

private:
	// NS에 노출된 진행률(0~1) Float User Parameter 이름 — 색상 계산(BaseColor * Ratio)은 NS/머티리얼 쪽에서 처리
	UPROPERTY(EditDefaultsOnly, Category = "Charge|Effect")
	FName ChargeRatioParameterName = TEXT("ChargeRatio");

	// 사이클 진행률에 따른 스케일 범위 (1사이클=Min, MaxCycle=Max)
	UPROPERTY(EditDefaultsOnly, Category = "Charge|Effect")
	float MinScale = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge|Effect")
	float MaxScale = 1.5f;

	// 인터럽트 등으로 마지막 사이클 도달 전에 어빌리티가 끝나는 경우를 대비한 정리용 — 설계 프로퍼티 아님(런타임 북키핑)
	FGameplayTag ActiveEffectTag;
};
