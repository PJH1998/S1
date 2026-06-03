// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBar/S1ProgressBar.h"
#include "S1BossStatus.generated.h"

class AS1BossBase;
class US1BossSet;
class UTextBlock;

/**
 * 
 */
UCLASS()
class S1_API US1BossStatus : public US1ProgressBar
{
	GENERATED_BODY()

public:
	US1BossStatus(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetBoss(AS1BossBase* InBoss);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	virtual void Bind_ShaderResource() override;

private:
	int32 GetLineCount(float InValue) const;
	float GetLineFillRatio(float InValue, int32 InLineCount) const;
	FLinearColor GetLineColor(int32 InLineCount) const;
	void UpdateLineCountText(int32 InLineCount);

private:
	float LerpValue = { 100.f };

	FName LerpRatioParameterName = TEXT("LerpRatio");
	FName CurrentColorParameterName = TEXT("CurrentColor");
	FName NextColorParameterName = TEXT("NextColor");

	UPROPERTY(EditAnywhere, Category = "BossStatus", meta = (AllowPrivateAccess = "true"))
	float HealthPerLine = { 100.f };

private:
	UPROPERTY(Transient)
	TObjectPtr<AS1BossBase> Boss;

	UPROPERTY(Transient)
	TObjectPtr<US1BossSet> BossSet;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Num;
};
