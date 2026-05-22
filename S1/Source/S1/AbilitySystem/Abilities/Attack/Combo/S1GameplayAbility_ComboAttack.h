// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "GameplayTagContainer.h"
#include "S1GameplayAbility_ComboAttack.generated.h"

UCLASS()
class S1_API US1GameplayAbility_ComboAttack : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

public:
	// 같은 GA 입력이 들어왔을 때 콤보 처리 — 소비했으면 true, 윈도우 미오픈이면 false
	virtual bool OnInputReactivated() override;

	// CanNextAttackTag를 큐 실행 트리거로 사용
	virtual FGameplayTag GetInputFlushTag() const override { return CanNextAttackTag; }

	// 다른 GA 입력이 들어왔을 때 전환 사용
	virtual bool OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// CurrentSectionIndex 사용
	virtual const FS1MontageSet* GetCurrentMontageSet() const override; 

private:
	void PlayMontageSet(const FGameplayAbilityActorInfo* ActorInfo);
	// 콤보 어드밴스 — 성공 시 true, CanNextAttackTag 없어 불가 시 false
	bool TryAdvanceCombo();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	// 콤보 전환 가능 여부 Tag ( AnimNotifyState_AttackCombo와 동일 태그로 설정 )
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FGameplayTag CanNextAttackTag;

	// 콤보 마지막에서 입력 시 전환할 AbilityTag ( ex) WeakAttackFinish -> Skill01 )
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FGameplayTag NextAttackAbilityTag;

	// Ability 전환 여부 Tag ( Tag를 보유한 GA 입력 시 전환 )
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FGameplayTagContainer CrossComboGroupTags;

	int32 CurrentSectionIndex = 0;
};
