// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance.h"
#include "S1PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1PlayerAnimInstance : public US1AnimInstance
{
	GENERATED_BODY()
	
public:
	US1PlayerAnimInstance(const FObjectInitializer& ObjectInitialzer = FObjectInitializer::Get());

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION()
	void AnimNotify_LoopStart();

	void SetCanNextAttack(bool bInCanNextAttack) { bCanNextAttack = bInCanNextAttack; }
	bool GetCanNextAttack() const { return bCanNextAttack; }

public:
	UPROPERTY(BlueprintReadOnly)
	bool bCanNextAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bSprint = false;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bRunLoop = false;
};
