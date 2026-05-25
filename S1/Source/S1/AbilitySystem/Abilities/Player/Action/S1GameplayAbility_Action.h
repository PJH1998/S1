// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Action.generated.h"

// 액션 중 상태(State.Action) 관리 + EarlyMove(이동/점프 입력 시 조기 종료) 공통 베이스
// Attack, Dash, Dodge 등 액션 GA의 부모 클래스
UCLASS()
class S1_API US1GameplayAbility_Action : public US1GameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// EarlyMove 구간 진입 — 이동/점프 입력 시 Montage_Stop + EndAbility
	// 필요 시 서브클래스에서 override 가능
	virtual void OnEarlyMoveEnabled();

private:
	UFUNCTION()
	void OnEarlyMoveEventReceived(FGameplayEventData Payload);

	void OnEarlyMoveTriggered();

private:
	// 액션 중 상태 태그 (ex. State.Action)
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FGameplayTag ActionStateTag;

	// 이 이벤트 수신 시 OnEarlyMoveEnabled 호출 (empty = EarlyMove 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "Action|EarlyMove")
	FGameplayTag EarlyMoveEventTag;

	// 점프 감지용 이벤트 태그 — Character::Jump()에서 SendGameplayEventToActor로 전송
	// (empty = 점프 감지 안 함)
	UPROPERTY(EditDefaultsOnly, Category = "Action|EarlyMove")
	FGameplayTag JumpEventTag;
};
