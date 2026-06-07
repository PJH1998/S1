// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_Dash.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;

// 입력 방향 수평 대쉬 — 4방향 애니메이션 분기, 공격으로 파생 가능
UCLASS()
class S1_API US1GameplayAbility_Dash : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	virtual bool OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnMoveBeginReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnMoveEndReceived(FGameplayEventData Payload);

	// 입력 방향과 캐릭터 전방의 각도로 4방향 섹션 이름 반환
	FName   ComputeDirectionalSection() const;
	// 입력 방향 반환 (없으면 액터 전방)
	FVector ComputeInputDirection() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashSpeed = 1500.f;

	// 대쉬 도중 이 태그 그룹의 Ability 입력 → 즉시 전환 허용 (윈도우 조건 없음)
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	FGameplayTagContainer AttackAbilityGroupTags;

	// MoveBegin 이벤트 태그 — ConstantForce 시작 + 중력 제어(bControlGravity) 트리거
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Move")
	FGameplayTag MoveBeginEventTag;

	// MoveEnd 이벤트 태그 — ConstantForce 종료 + 중력 복원 트리거
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Move")
	FGameplayTag MoveEndEventTag;

	// true: MoveBegin~MoveEnd 구간 GravityScale=0 (Air 대쉬용)
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Move")
	bool bControlGravity = false;

	// 4방향 애니메이션 섹션명 — BP에서 몽타주 섹션과 일치시킬 것
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Section")
	FName SectionForward = FName("F");

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Section")
	FName SectionBack = FName("B");

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Section")
	FName SectionLeft = FName("L");

	UPROPERTY(EditDefaultsOnly, Category = "Dash|Section")
	FName SectionRight = FName("R");

	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MoveTask;

	// ActivateAbility에서 캡처한 대쉬 방향 (MoveBegin 시 force 적용용)
	FVector CapturedDashDirection = FVector::ZeroVector;

	bool bCachedOrientToMovement = true;
};
