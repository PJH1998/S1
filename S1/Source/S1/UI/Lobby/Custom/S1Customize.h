// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Customize.generated.h"

class UImage;
class UButton;

/**
 *
 */
UCLASS()
class S1_API US1Customize : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1Customize(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnStartClicked();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CharacterModel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Start;
};
