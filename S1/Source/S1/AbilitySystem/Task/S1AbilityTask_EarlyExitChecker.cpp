// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

US1AbilityTask_EarlyExitChecker* US1AbilityTask_EarlyExitChecker::Run(UGameplayAbility* OwningAbility, FGameplayTag InJumpEventTag)
{
	US1AbilityTask_EarlyExitChecker* Task = NewAbilityTask<US1AbilityTask_EarlyExitChecker>(OwningAbility);
	Task->bTickingTask = true;
	Task->JumpEventTag = InJumpEventTag;
	return Task;
}

void US1AbilityTask_EarlyExitChecker::Activate()
{
	if (JumpEventTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(JumpEventTag)
				.AddUObject(this, &ThisClass::OnJumpEventReceived);
		}
	}
}

void US1AbilityTask_EarlyExitChecker::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ACharacter* Char = Cast<ACharacter>(GetAvatarActor());
	if (nullptr == Char)
	{
		return;
	}

	if (Char->GetCharacterMovement()->GetLastInputVector().SizeSquared() > KINDA_SMALL_NUMBER)
	{
		TriggerInputDetected();
	}
}

void US1AbilityTask_EarlyExitChecker::OnDestroy(bool bInOwnerFinished)
{
	if (JumpEventTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(JumpEventTag).RemoveAll(this);
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}

void US1AbilityTask_EarlyExitChecker::TriggerInputDetected()
{
	if (bTriggered)
	{
		return;
	}

	bTriggered = true;
	OnInputDetected.Broadcast();
	EndTask();
}

void US1AbilityTask_EarlyExitChecker::OnJumpEventReceived(const FGameplayEventData* Payload)
{
	TriggerInputDetected();
}
