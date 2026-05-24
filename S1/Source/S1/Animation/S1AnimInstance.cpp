// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/S1AnimInstance.h"
#include "Character/Player/S1Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1LogChannels.h"

US1AnimInstance::US1AnimInstance(const FObjectInitializer& ObjectInitialzer)
	: Super(ObjectInitialzer)
{
}

void US1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AS1Character>(TryGetPawnOwner());

	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void US1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == Character)
	{
		LOG_WARNING(TEXT("US1AnimInstance : Nullptr Character"));
		return;
	}

	if (nullptr == MovementComponent)
	{
		LOG_WARNING(TEXT("US1AnimInstance : Nullptr MovementComponent"));
		return;
	}

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	bMove = (GroundSpeed > 0.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);

	bIsFalling = MovementComponent->IsFalling();
}
