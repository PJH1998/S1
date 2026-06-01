// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BT_Task_Death.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Boss/S1BossBase.h"
#include "Character/S1Monster.h"
#include "Component/S1BossLocomotionComponent.h"
#include "Components/SkeletalMeshComponent.h"

UBT_Task_Death::UBT_Task_Death()
{
	NodeName = TEXT("Death");
	bCreateNodeInstance = true;
}

void UBT_Task_Death::SetBlackboardBool(UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& KeySelector, bool bValue) const
{
	if (KeySelector.SelectedKeyName == NAME_None)
	{
		return;
	}

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(KeySelector.SelectedKeyName, bValue);
	}
}

EBTNodeResult::Type UBT_Task_Death::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ClearMontageEndedDelegate();

	if (DeathAnimation == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	if (Monster == nullptr || Monster->IsDead() == false)
	{
		return EBTNodeResult::Failed;
	}

	if (AS1BossBase* Boss = Cast<AS1BossBase>(Monster))
	{
		if (US1BossLocomotionComponent* LocomotionComponent = Boss->GetLocomotionComponent())
		{
			LocomotionComponent->AbortApproach(true);
			LocomotionComponent->AbortTurn(true);
		}
	}

	SetBlackboardBool(OwnerComp, IsChasingKey, false);
	SetBlackboardBool(OwnerComp, IsTurningKey, false);

	Monster->HandleDeath();

	USkeletalMeshComponent* Mesh = Monster->GetMesh();
	if (Mesh == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (US1AnimInstance_BossBase* BossAnimInstance = Cast<US1AnimInstance_BossBase>(AnimInstance))
	{
		BossAnimInstance->ResetLocomotion();
	}

	ActiveDynamicMontage = AnimInstance->PlaySlotAnimationAsDynamicMontage(
		DeathAnimation,
		AnimationSlotName,
		BlendInTime,
		BlendOutTime);

	if (ActiveDynamicMontage == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;
	DeathMontageEndedDelegate.BindUObject(this, &UBT_Task_Death::OnDeathAnimationEnded);
	AnimInstance->Montage_SetEndDelegate(DeathMontageEndedDelegate, ActiveDynamicMontage);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBT_Task_Death::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ClearMontageEndedDelegate();
	return EBTNodeResult::Aborted;
}

void UBT_Task_Death::OnDeathAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveDynamicMontage)
	{
		return;
	}

	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;
	ClearMontageEndedDelegate();

	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
	}
}

void UBT_Task_Death::ClearMontageEndedDelegate()
{
	if (CachedOwnerComp && ActiveDynamicMontage)
	{
		if (AAIController* AIController = CachedOwnerComp->GetAIOwner())
		{
			if (AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn()))
			{
				if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
				{
					if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
					{
						FOnMontageEnded EmptyDelegate;
						AnimInstance->Montage_SetEndDelegate(EmptyDelegate, ActiveDynamicMontage);
					}
				}
			}
		}
	}

	DeathMontageEndedDelegate.Unbind();
	CachedOwnerComp = nullptr;
	ActiveDynamicMontage = nullptr;
}
