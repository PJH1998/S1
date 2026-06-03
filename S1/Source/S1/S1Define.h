// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define D(x) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, x); }

namespace S1Levels
{
	const FName LoadingMap = TEXT("LoadingMap");
}

namespace S1CollisionChannel
{
	constexpr ECollisionChannel CC_Player = ECC_GameTraceChannel1;
	constexpr ECollisionChannel CC_Monster = ECC_GameTraceChannel2;
	constexpr ECollisionChannel CC_Weapon = ECC_GameTraceChannel3;
	constexpr ECollisionChannel CC_LockOnSphere = ECC_GameTraceChannel4;
}
