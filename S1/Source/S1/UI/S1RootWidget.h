// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "S1RootWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeliverFinishedDelegate);
enum class UI_TYPE { HUD, MENU, POPUP, CURSOR, FADE, END };

class US1Cursor;
class US1Fade;
class US1Inventory_ItemInfo;

/**
 * Root UI 공통 베이스 — HUD/Cursor/Fade만 소유.
 * 레벨별 Root 위젯(Gameplay/Lobby)이 상속하여 확장한다.
 */
UCLASS(Abstract)
class S1_API US1RootWidget : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1RootWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetUp_Panel(UI_TYPE PanelType, const FGameplayTag& UITag);

	virtual void ShowMenu(const FGameplayTag& UITag) {}
	virtual void HideAllMenus() {}
	virtual bool IsInventoryMenuOpen() const { return false; }
	virtual US1Inventory_ItemInfo* GetItemInfoWidget() const { return nullptr; }

	// 상황별 텍스처 어나운스(퀘스트/기믹/보스 클리어 등). 레벨별로 필요한 곳(Gameplay)만 override해서 채운다.
	virtual void ShowAnnounce(const FGameplayTag& AnnounceTag) {}

	void FadeIn(float InDuration = 1.f);
	void FadeOut(float InDuration = 1.f);

	void SetCursorVisible(bool bVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

protected:
	template <typename T>
	T* Register_Panel(UI_TYPE type, TSubclassOf<T> SubClass, TObjectPtr<UCanvasPanel> CanvasPanel, bool bAutoSize = true);

private:
	void UpdateMousePosition(const FGeometry& MyGeometry);

	UCanvasPanel* GetCanvasPanel(UI_TYPE PanelType) const;

private:
	UFUNCTION()
	void DeliveFadeFinished();

public:
	FDeliverFinishedDelegate			DeliverFinished;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_HUD;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Cursor;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Fade;

private:
	TObjectPtr<US1Fade> FadeWidget;

	bool bCursorVisible = false;

protected:
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
