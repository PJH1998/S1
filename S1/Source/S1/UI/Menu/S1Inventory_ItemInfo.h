// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1Inventory_ItemInfo.generated.h"

class UTextBlock;

UCLASS()
class S1_API US1Inventory_ItemInfo : public US1BaseWidget
{
	GENERATED_BODY()

public:
	void SetItemInfo(FGameplayTag ItemTag);
	// AbsolutePosition은 FSlateApplication::GetCursorPos() 등의 데스크톱 절대 좌표 — 내부에서 캔버스 로컬로 변환한다.
	void ShowAt(const FVector2D& AbsolutePosition);
	void HideInfo();

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Stats;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Price;
};
