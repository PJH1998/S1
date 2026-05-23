// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Boss/S1BossBase.h"
#include "S1Boss_000.generated.h"

/*
[BOSS] Cobalt
 */
UCLASS()
class S1_API AS1Boss_000 : public AS1BossBase
{
	GENERATED_BODY()
	
public:
	AS1Boss_000();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
