// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "S1RootWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeliverFinishedDelegate);
enum class UI_TYPE { HUD, MENU, POPUP, CURSOR, FADE, END };

class US1HUD_Lobby;
class US1Cursor;
class US1Fade;
/**
 * 
 */
UCLASS()
class S1_API US1RootWidget : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1RootWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void FadeIn(float InDuration = 1.f);
	void FadeOut(float InDuration = 1.f);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	template <typename T>
	T* Register_Panel(UI_TYPE type, TSubclassOf<T> SubClass, TObjectPtr<UCanvasPanel> CanvasPanel, bool bAutoSize = true);

	void UpdateMousePosition(const FGeometry& MyGeometry);

private:
	UFUNCTION()
	void DeliveFadeFinished();

public:
	FDeliverFinishedDelegate			DeliverFinished;

private:
	UPROPERTY()
	TSubclassOf<US1HUD_Lobby>	HUDClass;
	UPROPERTY()
	TSubclassOf<US1Cursor>			CursorClass;
	UPROPERTY()
	TSubclassOf<US1Fade>			FadeClass;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_HUD;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Cursor;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Fade;

private:
	TObjectPtr<US1Fade> FadeWidget;

	TArray<TObjectPtr<UCanvasPanelSlot>> PanelSlots;
};

template <typename T>
T* US1RootWidget::Register_Panel(UI_TYPE type, TSubclassOf<T> SubClass, TObjectPtr<UCanvasPanel> CanvasPanel, bool bAutoSize)
{
	if (SubClass == nullptr || CanvasPanel == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create to Failed : UI"));
		return nullptr;
	}

	T* NewWidget = CreateWidget<T>(GetOwningPlayer(), SubClass);
	if (NewWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create to Failed : UI"));
		return nullptr;
	}

	int32 LocalType = static_cast<int32>(type);
	PanelSlots[LocalType] = CanvasPanel->AddChildToCanvas(NewWidget);
	PanelSlots[LocalType]->SetAutoSize(bAutoSize);

	return NewWidget;
}