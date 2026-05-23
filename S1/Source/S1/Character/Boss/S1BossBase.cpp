// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/S1BossBase.h"

AS1BossBase::AS1BossBase()
	: Super()
{
}

void AS1BossBase::NotifyHasTargetChanged(bool bInHasTarget)
{
	if (bHasTarget == bInHasTarget)
	{
		return;
	}

	bHasTarget = bInHasTarget;
	OnHasTargetChanged.Broadcast(this, bHasTarget);
}
