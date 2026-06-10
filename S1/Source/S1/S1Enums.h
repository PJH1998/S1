// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "S1Enums.generated.h"

UENUM(BlueprintType)
enum class ES1StencilLayer : uint8
{
	None   = 0 UMETA(DisplayName = "None"),
	Player = 1 UMETA(DisplayName = "Player"),
	Boss   = 2 UMETA(DisplayName = "Boss"),
};

UENUM(BlueprintType)
enum class ES1EnemyTier : uint8
{
	Boss   = 0 UMETA(DisplayName = "Boss"),
	Elite  = 1 UMETA(DisplayName = "Elite"),
	Normal = 2 UMETA(DisplayName = "Normal"),
};

UENUM(BlueprintType)
enum class ES1LockOnSide : uint8
{
	Right = 0 UMETA(DisplayName = "Right"),
	Left  = 1 UMETA(DisplayName = "Left"),
};

UENUM(BlueprintType)
enum class ES1WeaponType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	RPR  = 1 UMETA(DisplayName = "Rapier"),
	SWD  = 2 UMETA(DisplayName = "Sword"),
};
