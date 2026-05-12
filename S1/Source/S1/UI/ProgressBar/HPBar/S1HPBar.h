// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBar/S1ProgressBar.h"
#include "S1HPBar.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1HPBar : public US1ProgressBar
{
	GENERATED_BODY()
	
public:
	US1HPBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
};
