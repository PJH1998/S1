// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/S1AIController.h"

AS1AIController::AS1AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AS1AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//InitializeBlackboard();
	//RunBehaviorTree()
}

void AS1AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AS1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
