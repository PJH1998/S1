// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Cursor.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1Cursor : public US1BaseWidget
{
	GENERATED_BODY()
	
public:
	US1Cursor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

};
