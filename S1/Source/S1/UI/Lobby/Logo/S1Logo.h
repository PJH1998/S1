// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Logo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickedLogoDelegate);

class UTextBlock;
/**
 * 
 */
UCLASS()
class S1_API US1Logo : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1Logo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UFUNCTION()
	void Change_Customize();

public:
	UPROPERTY()
	FClickedLogoDelegate OnClickedLogoDelegate;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Logo;

private:
	float	OpacitySpeed = { 0.4f };
	int		OpacityReverse = { -1 };
	float	Opacity = { 1.f };
};
