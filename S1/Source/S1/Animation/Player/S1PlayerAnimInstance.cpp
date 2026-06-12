// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Player/S1PlayerAnimInstance.h"
#include "Character/Player/S1Player.h"

US1PlayerAnimInstance::US1PlayerAnimInstance(const FObjectInitializer& ObjectInitialzer)
	: Super(ObjectInitialzer)
{
}

void US1PlayerAnimInstance::OnLoopStartNotify()
{
	if (bMove)
	{
		bRunLoop = true;
	}
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

	if (false == bMove || bIsFalling)
	{
		bRunLoop = false;
	}

	if (!bIsFalling)
	{
		bDirectToFall = false;
	}

	//TEMP
	WaitTime = bMove ? 0.f : FMath::Min(WaitTime + DeltaSeconds, MaxWaitTime);

	if (WaitTime >= MaxWaitTime && false == bWait)
	{
		bWait = true;
		WaitTime = 0.f;
	}
	else
	{
		bWait = false;
	}
}

