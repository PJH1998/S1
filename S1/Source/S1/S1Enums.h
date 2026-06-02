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
