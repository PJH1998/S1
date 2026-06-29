// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "GameplayEffectTypes.h"
#include "S1GA_Boss000_JumpAttack.generated.h"

/**
 * Boss000 점프 슬램 패턴.
 * 몽타주(Start/Jump/Top/Fall/End)를 재생하고, 물리 신호로 섹션을 이동한다.
 *  - Event.Enemy.Apex  : 정점 도달 -> Top 섹션
 *  - Event.Enemy.Landed: 착지     -> End 섹션(슬램은 End의 AttackRange Notify가 처리)
 */
UCLASS()
class S1_API US1GA_Boss000_JumpAttack : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_JumpAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnApex(FGameplayEventData Payload);

	UFUNCTION()
	void OnLanded(FGameplayEventData Payload);

private:
	/** 정점 도달 시 점프할 섹션. */
	UPROPERTY(EditDefaultsOnly, Category = "JumpAttack")
	FName TopSectionName = FName("Top");

	/** 착지 시 점프할 섹션. */
	UPROPERTY(EditDefaultsOnly, Category = "JumpAttack")
	FName EndSectionName = FName("End");
};