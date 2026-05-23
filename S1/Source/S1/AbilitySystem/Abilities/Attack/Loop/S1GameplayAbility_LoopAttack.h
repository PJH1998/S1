// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_LoopAttack.generated.h"

UCLASS()
class S1_API US1GameplayAbility_LoopAttack : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

public:
	US1GameplayAbility_LoopAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool OnInputReactivated() override;
	virtual bool OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;
	virtual FGameplayTag GetInputFlushTag() const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Loop → End 섹션 전환 ( 필요시 Override )
	virtual void OnLoopEndEvent(FGameplayEventData Payload);

private:
	UFUNCTION()
	void OnLoopEndEventReceived(FGameplayEventData Payload); // AddDynamic 바인딩용, 내부에서 OnLoopEndEvent 호출

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	// Loop 종료 트리거 이벤트 태그 (e.g. Event.Landed)
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack")
	FGameplayTag LoopEndEventTag;

	// 다음 액션 전환 가능 윈도우 태그 (AnimNotifyState_LooseGameplayTag와 동일 태그 설정)
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack|CrossCombo")
	FGameplayTag CanNextAttackTag;

	// 전환 허용 Ability 그룹 태그
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack|CrossCombo")
	FGameplayTagContainer CrossComboGroupTags;

	// Start Section Name
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack|Section")
	FName StartSection = FName("Start");

	// Loop Section Name
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack|Section")
	FName LoopSection = FName("Loop");

	// End Section Name
	UPROPERTY(EditDefaultsOnly, Category = "LoopAttack|Section")
	FName EndSection = FName("End");
};
