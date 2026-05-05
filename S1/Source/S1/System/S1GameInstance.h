// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "S1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	US1GameInstance(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Init() override;
	virtual void Shutdown() override;
};
