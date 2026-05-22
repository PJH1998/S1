// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "S1GameplayAbility_Attack.generated.h"

struct FS1MontageData;
struct FS1MontageSet;
class US1AbilityTask_RotateToCamera;

UCLASS()
class S1_API US1GameplayAbility_Attack : public US1GameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	const FS1MontageData* GetMontageData() const;
	virtual const FS1MontageSet* GetCurrentMontageSet() const; // 기본: index 0

protected:
	// 실행할 Montage를 보관 중인 AnimData Asset Tag
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag AnimDataTag;

	// 실행할 Montage Asset Tag
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag MontageTag;

	// 특정 상황에서 재발동 방지를 위한 Tag ( 해당 Tag 부여 후 이후 Tag 삭제 필요 )
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag UsedTag;

	// US1AbilityTask_RotateToCamera 실행 여부
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	bool bRotateToCamera = false;

	// US1AbilityTask_RotateToCamera 회전 속도
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (EditCondition = "bRotateToCamera"))
	float RotationSpeed = 720.f;

protected:
	void StartRotateToCamera();

private:
	UPROPERTY()
	TObjectPtr<US1AbilityTask_RotateToCamera> RotateTask;
};
