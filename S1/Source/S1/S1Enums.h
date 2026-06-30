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
	DSWD = 3 UMETA(DisplayName = "DualSword"),
};

UENUM(BlueprintType)
enum class EPlayerGender : uint8
{
	Male   = 0 UMETA(DisplayName = "Male"),
	Female = 1 UMETA(DisplayName = "Female"),
};

UENUM(BlueprintType)
enum class ES1Direction : uint8
{
	Forward = 0 UMETA(DisplayName = "Forward"),
	Back    = 1 UMETA(DisplayName = "Back"),
	Left    = 2 UMETA(DisplayName = "Left"),
	Right   = 3 UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class ES1HitReactType : uint8
{
	None    = 0 UMETA(DisplayName = "None"),
	Weak    = 1 UMETA(DisplayName = "Weak"),
	Strong  = 2 UMETA(DisplayName = "Strong"),
	Launch  = 3 UMETA(DisplayName = "Launch"),
};

UENUM(BlueprintType)
enum class ES1AttackHand : uint8
{
	Main    = 0 UMETA(DisplayName = "Main Hand"),
	Offhand = 1 UMETA(DisplayName = "Off Hand"),
};
