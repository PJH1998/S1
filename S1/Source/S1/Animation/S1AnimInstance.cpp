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
		return;
	}

	if (false == IsValid(MovementComponent))
	{
		return;
	}

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	bMove = (GroundSpeed > 0.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);

	bIsFalling = MovementComponent->IsFalling();
}

void US1AnimInstance::RequestInertialization(float BlendTime)
{
	LOG(TEXT("[Inertialization] RequestInertialization 호출 — Class: %s, BlendTime: %f"), *GetClass()->GetName(), BlendTime);

	IAnimClassInterface* AnimClassInterface = IAnimClassInterface::GetFromClass(GetClass());
	if (nullptr == AnimClassInterface)
	{
		LOG(TEXT("[Inertialization] AnimClassInterface 못 찾음"));
		return;
	}

	int32 FoundCount = 0;
	for (FStructProperty* NodeProperty : AnimClassInterface->GetAnimNodeProperties())
	{
		if (NodeProperty->Struct == FAnimNode_Inertialization::StaticStruct())
		{
			FoundCount++;
			if (FAnimNode_Inertialization* Node = NodeProperty->ContainerPtrToValuePtr<FAnimNode_Inertialization>(this))
			{
				LOG(TEXT("[Inertialization] 노드 발견(%d번째, 프로퍼티명: %s) — RequestInertialization 실행"), FoundCount, *NodeProperty->GetName());
				Node->RequestInertialization(BlendTime, nullptr);
				return;
			}
		}
	}

	LOG(TEXT("[Inertialization] Inertialization 노드를 그래프에서 못 찾음 (FoundCount: %d)"), FoundCount);
}
