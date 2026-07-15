// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "S1Enums.generated.h"

UENUM(BlueprintType)
enum class ES1StencilLayer : uint8
{
	None             = 0 UMETA(DisplayName = "None"),
	Player           = 1 UMETA(DisplayName = "Player"),
	Boss             = 2 UMETA(DisplayName = "Boss"),

	// Ultimate 컷씬 "플레이어만 보이기" PP 전용 — 멀티에서 시전자 이외의 다른 Player(모두 stencil=1 공유)가
	// 같이 하이라이트되는 것 방지. 시전 중에만 시전자 메시에 이 값을 임시로 덮어씀(UltiCleanup에서 Player로 복구)
	UltimateSpotlight = 3 UMETA(DisplayName = "UltimateSpotlight"),
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

UENUM(BlueprintType)
enum class ES1EffectAttachTarget : uint8
{
	Character      = 0 UMETA(DisplayName = "Character"),
	MainHandWeapon = 1 UMETA(DisplayName = "Main Hand Weapon"),
	OffHandWeapon  = 2 UMETA(DisplayName = "Off Hand Weapon"),
};

UENUM(BlueprintType)
enum class ES1AtkCollisionShape : uint8
{
	None       = 0 UMETA(DisplayName = "None"),
	Sphere     = 1 UMETA(DisplayName = "Sphere"),
	Box        = 2 UMETA(DisplayName = "Box"),
	Hemisphere = 3 UMETA(DisplayName = "Hemisphere"),
};
