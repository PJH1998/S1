// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "S1AnimInstance_Boss000.generated.h"

UCLASS()
class S1_API US1AnimInstance_Boss000 : public US1AnimInstance_BossBase
{
	GENERATED_BODY()

public:
	US1AnimInstance_Boss000(const FObjectInitializer& ObjectInitialzer = FObjectInitializer::Get());
};
