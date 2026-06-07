// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/S1AnimInstance.h"
#include "Character/Player/S1Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1LogChannels.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Animation/AnimClassInterface.h"

US1AnimInstance::US1AnimInstance(const FObjectInitializer& ObjectInitialzer)
	: Super(ObjectInitialzer)
{
}

void US1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AS1Character>(TryGetPawnOwner());

	if (IsValid(Character))
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void US1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (false == IsValid(Character))
	{
		LOG_WARNING(TEXT("US1AnimInstance : Nullptr Character"));
		return;
	}

	if (false == IsValid(MovementComponent))
	{
		LOG_WARNING(TEXT("US1AnimInstance : Nullptr MovementComponent"));
		return;
	}

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	bMove = (GroundSpeed > 0.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);

	bIsFalling = MovementComponent->IsFalling();
}

void US1AnimInstance::RequestInertialization(float BlendTime)
{
	IAnimClassInterface* AnimClassInterface = IAnimClassInterface::GetFromClass(GetClass());
	if (nullptr == AnimClassInterface)
	{
		return;
	}

	for (FStructProperty* NodeProperty : AnimClassInterface->GetAnimNodeProperties())
	{
		if (NodeProperty->Struct == FAnimNode_Inertialization::StaticStruct())
		{
			if (FAnimNode_Inertialization* Node = NodeProperty->ContainerPtrToValuePtr<FAnimNode_Inertialization>(this))
			{
				Node->RequestInertialization(BlendTime, nullptr);
				return;
			}
		}
	}
}
