// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBar/S1ProgressBar.h"
#include "S1ExpBar.generated.h"

class US1PlayerSet;
class UTextBlock;
/**
 *
 */
UCLASS()
class S1_API US1ExpBar : public US1ProgressBar
{
	GENERATED_BODY()

public:
	US1ExpBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	virtual void Bind_ShaderResource() override;

private:
	void UpdateLevelText(int32 InLevel);

private:
	float LerpValue = { 100.f };

	FName LerpRatioParameterName = TEXT("LerpRatio");

private:
	TObjectPtr<US1PlayerSet> PlayerSet;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Level;
};
