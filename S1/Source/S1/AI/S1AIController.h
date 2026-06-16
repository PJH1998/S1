// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "S1Enums.h"
#include "S1AIController.generated.h"

class AS1Monster;

UCLASS()
class S1_API AS1AIController : public AAIController
{
	GENERATED_BODY()

public:
	AS1AIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;
	AActor* GetTargetActor() const;
	void SetBlackboardIsDead(bool bInIsDead);
	void SetHitReactRequest(ES1HitReactType HitType, ES1Direction HitDirection, AActor* Attacker);
	void ClearHitReactRequest();
	void StopAIForDeath();
	void ResumeAIAfterRevive();
	void ResetBlackboardForSpawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetKeyName = TEXT("TargetActor");

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName IsDeadKeyName = TEXT("bIsDead");

	UPROPERTY(EditDefaultsOnly, Category = "AI|HitReact")
	FName PendingHitReactKeyName = TEXT("bPendingHitReact");

	UPROPERTY(EditDefaultsOnly, Category = "AI|HitReact")
	FName HitTypeKeyName = TEXT("HitType");

	UPROPERTY(EditDefaultsOnly, Category = "AI|HitReact")
	FName HitDirectionKeyName = TEXT("HitDirection");

	UPROPERTY(EditDefaultsOnly, Category = "AI|HitReact")
	FName HitAttackerKeyName = TEXT("HitAttacker");
};
