// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Evasion/S1GameplayAbility_Evasion.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1GameplayAbility_Dash.generated.h"

// 입력 방향 수평 대쉬 — 4방향 애니메이션 분기, 공격으로 파생 가능
// 이동 처리 (MoveTask/GravityScale/OrientRotation)는 Evasion 베이스에서 담당
UCLASS()
class S1_API US1GameplayAbility_Dash : public US1GameplayAbility_Evasion
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Dash();

	virtual bool OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
		// 입력 방향과 캐릭터 전방의 각도로 4방향 Enum 반환
	ES1Direction ComputeDirection() const;

private:
	// 대쉬 도중 이 태그 그룹의 Ability 입력 → 즉시 전환 허용 (윈도우 조건 없음)
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	FGameplayTagContainer AttackAbilityGroupTags;
};
