// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "S1RootWidget.generated.h"

enum class UI_TYPE { HUD, MENU, POPUP, CURSOR, END };

class US1HUD_Lobby;
class US1Cursor;
/**
 * 
 */
UCLASS()
class S1_API US1RootWidget : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1RootWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	template <typename T>
	void Register_Panel(UI_TYPE type, TSubclassOf<T> SubClass, bool bAutoSize = true);

	void UpdateMousePosition(const FGeometry& MyGeometry);

private:
	UPROPERTY()
	TSubclassOf<US1HUD_Lobby> HUDClass;
	UPROPERTY()
	TSubclassOf<US1Cursor> CursorClass;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_HUD;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Cursor;

private:
	TArray<TObjectPtr<UCanvasPanelSlot>> PanelSlots;
};

template <typename T>
void US1RootWidget::Register_Panel(UI_TYPE type, TSubclassOf<T> SubClass, bool bAutoSize)
{
	T* NewWidget = CreateWidget<T>(GetOwningPlayer(), SubClass);
	if (NewWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create to Failed : UI"));
		return;
	}

	switch (type)
	{
	case UI_TYPE::HUD:
		PanelSlots[static_cast<int32>(type)] = CanvasPanel_HUD->AddChildToCanvas(NewWidget);
		break;
	case UI_TYPE::CURSOR:
		PanelSlots[static_cast<int32>(type)] = CanvasPanel_Cursor->AddChildToCanvas(NewWidget);
		break;
	}
	PanelSlots[static_cast<int32>(type)]->SetAutoSize(bAutoSize);
	if (bAutoSize == false)
	{
		PanelSlots[static_cast<int32>(type)]->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		PanelSlots[static_cast<int32>(type)]->SetOffsets(FMargin(0.0f));
		PanelSlots[static_cast<int32>(type)]->SetAlignment(FVector2D::ZeroVector);
	}
}