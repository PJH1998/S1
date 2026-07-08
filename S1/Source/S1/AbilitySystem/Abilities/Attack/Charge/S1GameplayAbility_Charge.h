// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Charge.generated.h"

// 홀드 차징 스킬 — RepeatLoop 계열 Progression의 사이클 완료마다 이펙트를 갱신
// 스폰/색상/소켓 설정은 이 GA가 아니라 LoopMontage에 배치한 US1AnimNotify_SpawnLoopCycleEffect가 담당
// (캐릭터별로 이미 갈라져있는 몽타주 쪽에서 설정 — GA/NS는 공용 재사용). 그 Notify가 사이클 완료 이벤트에
// EffectTag를 실어 보내면 Progression을 거쳐 이 훅으로 전달됨 — 사이클 카운트는 Progression에만 존재(중복 없음)
// 1사이클: 해당 EffectTag로 등록된 컴포넌트를 찾아 재트리거 / NS는 Loop Behavior=Once + Burst 스폰으로 세팅
UCLASS()
class S1_API US1GameplayAbility_Charge : public US1GameplayAbility_Attack
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
