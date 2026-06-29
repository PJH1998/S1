// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "S1GA_Boss000_Guard.generated.h"

/**
 * 보스 가드 어빌리티. BT 방어 패턴이 발동한다.
 * ActivateAbility에서 State.Boss.Guard loose 태그를 부여하고 가드 몽타주를 재생하며,
 * EndAbility(취소/중단 포함)에서 태그를 제거한다. 정면 데미지 차단 판정은 US1BossSet가 이 태그를 읽어 처리.
 */
UCLASS()
class S1_API US1GA_Boss000_Guard : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_Guard(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	MontageTag_Phase01;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag	MontageTag_Phase02;
};
