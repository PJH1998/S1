// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BT_Task_BackJump.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_BackJump::UBT_Task_BackJump()
{
	NodeName = TEXT("BackJump");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBT_Task_BackJump::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	ClearCachedState();

	if (BackJumpMontage == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	if (Monster == nullptr || Monster->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	if (US1EnemyLocomotionComponent* LocomotionComponent = Monster->GetLocomotionComponent())
	{
		LocomotionComponent->AbortApproach(true);
	}

	AIController->StopMovement();

	if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
	{
		if (US1AnimInstance_EnemyLocomotion* AnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()))
		{
			AnimInstance->StopTurnMontage();
		}
	}

	const float MontageDuration = Monster->PlayMonsterMontage(BackJumpMontage, 1.f, StartSection);
	if (MontageDuration <= 0.f)
	{
		return EBTNodeResult::Failed;
	}

	CachedMonster = Monster;
	return EBTNodeResult::InProgress;
}

void UBT_Task_BackJump::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTaskFinished)
	{
		return;
	}

	AS1Monster* Monster = CachedMonster;
	if (Monster == nullptr || Monster->IsDead())
	{
		bTaskFinished = true;
		ClearCachedState();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance == nullptr || !AnimInstance->Montage_IsPlaying(BackJumpMontage))
	{
		bTaskFinished = true;
		ClearCachedState();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	Monster->AddMovementInput(-Monster->GetActorForwardVector(), BackwardMovementInputScale * DeltaSeconds);
}

EBTNodeResult::Type UBT_Task_BackJump::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (bStopMontageOnAbort)
	{
		if (AS1Monster* Monster = CachedMonster)
		{
			if (UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr)
			{
				AnimInstance->Montage_Stop(0.1f, BackJumpMontage);
			}
		}
	}

	bTaskFinished = true;
	ClearCachedState();
	return EBTNodeResult::Aborted;
}

void UBT_Task_BackJump::ClearCachedState()
{
	CachedMonster = nullptr;
}
