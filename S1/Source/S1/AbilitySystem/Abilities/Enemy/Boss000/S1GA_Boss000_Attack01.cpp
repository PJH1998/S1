// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Enemy/Boss000/S1GA_Boss000_Attack01.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AI/S1AIController.h"
#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "Engine/World.h"
#include "TimerManager.h"

US1GA_Boss000_Attack01::US1GA_Boss000_Attack01(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_Attack01::OnInputReactivated()
{
	Super::OnInputReactivated();
}

void US1GA_Boss000_Attack01::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == IsActive())
	{
		return;
	}

	const FS1MontageSet* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(ActorInfo->AvatarActor);
	if (nullptr == Monster)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (nullptr == AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveMontage = MontageSet->Montage;

	const float Duration = Monster->PlayAnimMontage(ActiveMontage, 1.f, StartSection);
	if (Duration <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AnimInstance->Montage_SetNextSection(StartSection, LoopSection, ActiveMontage);
	AnimInstance->Montage_SetNextSection(LoopSection, EndSection, ActiveMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ActiveMontage);

	if (CheckDistanceEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CheckDistanceEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnCheckDistanceEvent);
		Task->ReadyForActivation();
	}

	if (UWorld* World = Monster->GetWorld())
	{
		World->GetTimerManager().SetTimer(RotateTimerHandle, this, &ThisClass::RotateToTargetInLoop, RotationInterval, true);
	}
}

void US1GA_Boss000_Attack01::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RotateTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GA_Boss000_Attack01::OnCheckDistanceEvent(FGameplayEventData Payload)
{
	UpdateNextSectionByDistance();
}

void US1GA_Boss000_Attack01::UpdateNextSectionByDistance()
{
	AS1Monster* Monster = Cast<AS1Monster>(GetAvatarActorFromActorInfo());
	UAnimInstance* AnimInstance = Monster ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == AnimInstance || nullptr == ActiveMontage)
	{
		return;
	}

	const FName NextSection = GetDistanceToTarget() > LoopDistance ? LoopSection : EndSection;

	AnimInstance->Montage_SetNextSection(StartSection, NextSection, ActiveMontage);
	AnimInstance->Montage_SetNextSection(LoopSection, EndSection, ActiveMontage);
}

void US1GA_Boss000_Attack01::RotateToTargetInLoop()
{
	AS1Monster* Monster = Cast<AS1Monster>(GetAvatarActorFromActorInfo());
	UAnimInstance* AnimInstance = Monster ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == Monster || nullptr == AnimInstance || nullptr == ActiveMontage)
	{
		return;
	}

	if (AnimInstance->Montage_GetCurrentSection(ActiveMontage) != LoopSection)
	{
		return;
	}

	const AActor* TargetActor = GetTargetActor();
	if (nullptr == TargetActor)
	{
		return;
	}

	FVector Direction = TargetActor->GetActorLocation() - Monster->GetActorLocation();
	Direction.Z = 0.f;
	if (false == Direction.Normalize())
	{
		return;
	}

	const FRotator CurrentRotation = Monster->GetActorRotation();
	const FRotator TargetRotation = Direction.Rotation();
	const FRotator NextRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, RotationInterval, RotationSpeed);

	Monster->SetActorRotation(FRotator(0.f, NextRotation.Yaw, 0.f));
}

float US1GA_Boss000_Attack01::GetDistanceToTarget() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (nullptr == AvatarActor)
	{
		return 0.f;
	}

	const AActor* TargetActor = GetTargetActor();
	if (nullptr == TargetActor)
	{
		return 0.f;
	}

	return FVector::Dist2D(AvatarActor->GetActorLocation(), TargetActor->GetActorLocation());
}

AActor* US1GA_Boss000_Attack01::GetTargetActor() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const AS1AIController* AIController = OwnerPawn ? Cast<AS1AIController>(OwnerPawn->GetController()) : nullptr;
	return AIController ? AIController->GetTargetActor() : nullptr;
}
