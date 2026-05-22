// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "S1AIController.generated.h"

/**
 * 
 */
UCLASS()
class S1_API AS1AIController : public AAIController
{
	GENERATED_BODY()

public:
	AS1AIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;
	AActor* GetTargetActor() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetKeyName = TEXT("TargetActor");
};
