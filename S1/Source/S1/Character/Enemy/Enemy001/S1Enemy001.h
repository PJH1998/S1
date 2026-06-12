// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1NormalMonster.h"
#include "S1Enemy001.generated.h"

UCLASS()
class S1_API AS1Enemy001 : public AS1NormalMonster
{
	GENERATED_BODY()

public:
	AS1Enemy001();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Chase")
	float DefaultMaxWalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Chase")
	float DefaultMaxAcceleration = 2048.f;
};
