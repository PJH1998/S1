// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/S1PlayerAnimInstance.h"
#include "Character/S1Player.h"

US1PlayerAnimInstance::US1PlayerAnimInstance(const FObjectInitializer& ObjectInitialzer)
	: Super(ObjectInitialzer)
{
}

void US1PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void US1PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (AS1Player* Player = Cast<AS1Player>(Character))
	{
		bSprint = Player->GetSprinting();
	}

	if (false == bMove)
	{
		bRunLoop = false;
	}
}

void US1PlayerAnimInstance::AnimNotify_LoopStart()
{
	if (bMove)
	{
		bRunLoop = true;
	}
}
