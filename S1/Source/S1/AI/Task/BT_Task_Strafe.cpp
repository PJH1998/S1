// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BT_Task_Strafe.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1NormalMonster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_Strafe::UBT_Task_Strafe()
{
	NodeName = TEXT("Strafe");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBT_Task_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	ClearCachedState();

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AS1NormalMonster* Monster = Cast<AS1NormalMonster>(AIController->GetPawn());
	if (Monster == nullptr || Monster->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ActiveStrafeDirection = ClassifyStrafeDirection(Monster, Target);
	ActiveStrafeMontage = GetStrafeMontageForDirection(ActiveStrafeDirection);
	if (ActiveStrafeMontage == nullptr)
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

	const float MontageDuration = Monster->PlayAnimMontage(ActiveStrafeMontage, 1.f, StartSection);
	if (MontageDuration <= 0.f)
	{
		ClearCachedState();
		return EBTNodeResult::Failed;
	}

	CachedMonster = Monster;
	return EBTNodeResult::InProgress;
}

void UBT_Task_Strafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTaskFinished)
	{
		return;
	}

	AS1NormalMonster* Monster = CachedMonster;
	if (Monster == nullptr || Monster->IsDead())
	{
		bTaskFinished = true;
		ClearCachedState();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance == nullptr || ActiveStrafeMontage == nullptr || !AnimInstance->Montage_IsPlaying(ActiveStrafeMontage))
	{
		bTaskFinished = true;
		ClearCachedState();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (IsStrafeLoopSection(AnimInstance))
	{
		Monster->AddMovementInput(Monster->GetActorRightVector(), GetStrafeDirectionScalar() * MovementInputScale * DeltaSeconds);
	}
}

EBTNodeResult::Type UBT_Task_Strafe::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (bStopMontageOnAbort)
	{
		if (AS1NormalMonster* Monster = CachedMonster)
		{
			if (UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr)
			{
				AnimInstance->Montage_Stop(0.1f, ActiveStrafeMontage);
			}
		}
	}

	bTaskFinished = true;
	ClearCachedState();
	return EBTNodeResult::Aborted;
}

float UBT_Task_Strafe::GetStrafeDirectionScalar() const
{
	return ActiveStrafeDirection == ES1StrafeDirection::Left ? -1.f : 1.f;
}

ES1StrafeDirection UBT_Task_Strafe::ClassifyStrafeDirection(AS1NormalMonster* Monster, AActor* Target) const
{
	if (Monster == nullptr || Target == nullptr)
	{
		return ES1StrafeDirection::Right;
	}

	FVector ToTarget = Target->GetActorLocation() - Monster->GetActorLocation();
	ToTarget.Z = 0.f;
	if (ToTarget.Normalize() == false)
	{
		return ES1StrafeDirection::Right;
	}

	FVector Right = Monster->GetActorRightVector();
	Right.Z = 0.f;
	if (Right.Normalize() == false)
	{
		return ES1StrafeDirection::Right;
	}

	return FVector::DotProduct(Right, ToTarget) < 0.f ? ES1StrafeDirection::Left : ES1StrafeDirection::Right;
}

UAnimMontage* UBT_Task_Strafe::GetStrafeMontageForDirection(ES1StrafeDirection Direction) const
{
	return Direction == ES1StrafeDirection::Left ? LeftStrafeMontage : RightStrafeMontage;
}

bool UBT_Task_Strafe::IsStrafeLoopSection(const UAnimInstance* AnimInstance) const
{
	if (AnimInstance == nullptr || ActiveStrafeMontage == nullptr)
	{
		return false;
	}

	return AnimInstance->Montage_GetCurrentSection(ActiveStrafeMontage) == LoopSection;
}

void UBT_Task_Strafe::ClearCachedState()
{
	CachedMonster = nullptr;
	ActiveStrafeMontage = nullptr;
	ActiveStrafeDirection = ES1StrafeDirection::Right;
}
