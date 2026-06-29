// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "S1GA_Boss000_PhaseTransition.generated.h"

/**
 * 보스 HP 50% 미만 시 1회 발동하는 페이즈 전환 어빌리티.
 * 전환 몽타주를 재생하고, 몽타주 노티파이(던지기/뽑기) 타이밍에 맞춰 무기를 숨겼다가 Sword(2페이즈)로 교체한다.
 * 블랙보드 BossPhase=2는 뽑는 시점/종료 시점에 갱신하고, 펜딩 해제는 발동 즉시 수행한다.
 */
UCLASS()
class S1_API US1GA_Boss000_PhaseTransition : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_PhaseTransition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 무기 날아가는 순간 -> 무기 숨김.
	UFUNCTION()
	void OnWeaponHide(FGameplayEventData Payload);

	// 다시 뽑는 순간 -> Sword로 교체 + 표시 + 페이즈 커밋.
	UFUNCTION()
	void OnWeaponDraw(FGameplayEventData Payload);

private:
	// 몽타주가 실제로 재생됐을 때만 종료 시 최종 상태(Sword/표시/페이즈2)를 보장한다.
	bool bCommitOnEnd = false;
};