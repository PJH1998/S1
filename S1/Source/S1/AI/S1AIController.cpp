// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/S1AIController.h"
#include "S1LogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/S1Monster.h"

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

void AS1AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AS1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
