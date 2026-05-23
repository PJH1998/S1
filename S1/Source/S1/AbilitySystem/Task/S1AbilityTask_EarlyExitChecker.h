// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "S1AbilityTask_EarlyExitChecker.generated.h"

UCLASS()
class S1_API US1AbilityTask_EarlyExitChecker : public UAbilityTask
{
	GENERATED_BODY()

public:
	// InJumpEventTag: 유효 시 해당 이벤트 수신 시에도 감지 (empty = 점프 감지 안 함)
	static US1AbilityTask_EarlyExitChecker* Run(UGameplayAbility* OwningAbility, FGameplayTag InJumpEventTag = FGameplayTag());

	DECLARE_MULTICAST_DELEGATE(FOnInputDetected);
	FOnInputDetected OnInputDetected;

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void TriggerInputDetected();
	void OnJumpEventReceived(const FGameplayEventData* Payload);

	FGameplayTag JumpEventTag;
	bool bTriggered = false;
};
