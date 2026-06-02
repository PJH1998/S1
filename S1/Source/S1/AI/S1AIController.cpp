// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/S1AIController.h"
#include "S1LogChannels.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/S1Monster.h"
#include "BrainComponent.h"

AS1AIController::AS1AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AS1AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AS1Monster* Monster = Cast<AS1Monster>(InPawn);
	if (Monster == nullptr)
	{
		LOG_ERROR(TEXT("Can't Find Monster"));
		return;
	}
	UBehaviorTree* BehaviorTree = Monster->GetBehaviorTree();
	if (BehaviorTree == nullptr)
	{
		LOG_ERROR(TEXT("Can't Find BT"));
		return;
	}
	RunBehaviorTree(BehaviorTree);
}

AActor* AS1AIController::GetTargetActor() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return nullptr;
	}
	return Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeyName));
}

void AS1AIController::SetBlackboardIsDead(bool bInIsDead)
{
	if (IsDeadKeyName == NAME_None)
	{
		return;
	}
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsBool(IsDeadKeyName, bInIsDead);
	}
}

void AS1AIController::StopAIForDeath()
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StopLogic(TEXT("Dead"));
	}
}

void AS1AIController::ResumeAIAfterRevive()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AS1Monster* Monster = Cast<AS1Monster>(ControlledPawn))
		{
			if (UBehaviorTree* BehaviorTree = Monster->GetBehaviorTree())
			{
				RunBehaviorTree(BehaviorTree);
			}
		}
	}
}

void AS1AIController::ResetBlackboardForSpawn()
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		SetBlackboardIsDead(false);
		if (TargetKeyName != NAME_None)
		{
			BlackboardComponent->ClearValue(TargetKeyName);
		}
	}
}

void AS1AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AS1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
