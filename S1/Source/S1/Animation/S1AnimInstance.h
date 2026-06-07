// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "S1AnimInstance.generated.h"

class AS1Character;
class UCharacterMovementComponent;

UCLASS()
class S1_API US1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	US1AnimInstance(const FObjectInitializer& ObjectInitialzer = FObjectInitializer::Get());

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// AnimGraph Inertialization 노드에 블렌드 요청 (섹션/몽타주 전환 시 하드컷 방지)
	void RequestInertialization(float BlendTime);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AS1Character> Character;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bMove = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;
};
