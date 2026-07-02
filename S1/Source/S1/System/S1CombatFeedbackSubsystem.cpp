// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1CombatFeedbackSubsystem.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/S1UIData.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "System/S1AssetManager.h"
#include "System/S1UIManager.h"
#include "Tags/S1GameplayTags.h"
#include "UI/Gameplay/DamageNumber/S1DamageNumberWidget.h"
#include "UI/Gameplay/S1RootWidget_Gameplay.h"

void US1CombatFeedbackSubsystem::Deinitialize()
{
	for (FS1ActiveFeedbackNumber& ActiveNumber : ActiveNumbers)
	{
		ReleaseWidget(ActiveNumber.Widget);
	}
	ActiveNumbers.Reset();
	PooledWidgets.Reset();
	Super::Deinitialize();
}

void US1CombatFeedbackSubsystem::Tick(float DeltaTime)
{
	for (int32 Index = ActiveNumbers.Num() - 1; Index >= 0; --Index)
	{
		FS1ActiveFeedbackNumber& ActiveNumber = ActiveNumbers[Index];
		ActiveNumber.ElapsedTime += DeltaTime;

		if (ActiveNumber.ElapsedTime >= ActiveNumber.Lifetime)
		{
			ReleaseWidget(ActiveNumber.Widget);
			ActiveNumbers.RemoveAtSwap(Index);
			continue;
		}

		UpdateActiveNumber(ActiveNumber);
	}
}

TStatId US1CombatFeedbackSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(US1CombatFeedbackSubsystem, STATGROUP_Tickables);
}

bool US1CombatFeedbackSubsystem::IsTickable() const
{
	return ActiveNumbers.Num() > 0;
}

void US1CombatFeedbackSubsystem::ShowDamageNumber(int32 Damage, const FVector& HitLocation)
{
	TArray<int32> Digits;
	SplitDamageIntoDigits(FMath::Abs(Damage), Digits);
	if (Digits.IsEmpty())
	{
		return;
	}

	US1DamageNumberWidget* Widget = AcquirePooledWidget();
	if (Widget == nullptr)
	{
		return;
	}

	Widget->ShowDigits(Digits);
	RegisterActiveNumber(Widget, HitLocation);
}

void US1CombatFeedbackSubsystem::ShowBlockNumber(const FVector& HitLocation)
{
	US1DamageNumberWidget* Widget = AcquirePooledWidget();
	if (Widget == nullptr)
	{
		return;
	}

	Widget->ShowBlock();
	RegisterActiveNumber(Widget, HitLocation);
}

void US1CombatFeedbackSubsystem::SplitDamageIntoDigits(int32 Damage, TArray<int32>& OutDigits)
{
	OutDigits.Reset();

	int32 RemainingDamage = FMath::Abs(Damage);
	if (RemainingDamage == 0)
	{
		OutDigits.Add(0);
		return;
	}

	while (RemainingDamage > 0)
	{
		OutDigits.Insert(RemainingDamage % 10, 0);
		RemainingDamage /= 10;
	}
}

FVector US1CombatFeedbackSubsystem::GetRandomizedLocation(const FVector& HitLocation) const
{
	const float RandomAngle = FMath::FRandRange(0.f, 2.f * UE_PI);
	const float RandomDistance = FMath::FRandRange(0.f, RandomRadius);
	const FVector RandomOffset(
		FMath::Cos(RandomAngle) * RandomDistance,
		FMath::Sin(RandomAngle) * RandomDistance,
		0.f);

	return HitLocation + RandomOffset;
}

UCanvasPanel* US1CombatFeedbackSubsystem::GetOrResolveDamageNumberCanvas()
{
	if (CachedDamageNumberCanvas.IsValid())
	{
		return CachedDamageNumberCanvas.Get();
	}

	UWorld* World = GetWorld();
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
	US1UIManager* UIManager = LocalPlayer ? LocalPlayer->GetSubsystem<US1UIManager>() : nullptr;
	US1RootWidget_Gameplay* RootWidget = UIManager ? Cast<US1RootWidget_Gameplay>(UIManager->GetRootWidget()) : nullptr;
	if (RootWidget == nullptr)
	{
		return nullptr;
	}

	UCanvasPanel* Canvas = RootWidget->GetDamageNumberCanvas();
	CachedDamageNumberCanvas = Canvas;
	return Canvas;
}

US1DamageNumberWidget* US1CombatFeedbackSubsystem::AcquirePooledWidget()
{
	if (PooledWidgets.Num() > 0)
	{
		return PooledWidgets.Pop();
	}

	UCanvasPanel* Canvas = GetOrResolveDamageNumberCanvas();
	if (Canvas == nullptr)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	if (PC == nullptr)
	{
		return nullptr;
	}

	const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData);
	if (UIData == nullptr)
	{
		return nullptr;
	}

	const TSubclassOf<US1BaseWidget> WidgetClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Gameplay_DamageNumber);
	if (WidgetClass == nullptr)
	{
		return nullptr;
	}

	US1DamageNumberWidget* NewWidget = CreateWidget<US1DamageNumberWidget>(PC, WidgetClass);
	if (NewWidget == nullptr)
	{
		return nullptr;
	}

	Canvas->AddChildToCanvas(NewWidget);
	return NewWidget;
}

void US1CombatFeedbackSubsystem::RegisterActiveNumber(US1DamageNumberWidget* Widget, const FVector& HitLocation)
{
	FS1ActiveFeedbackNumber ActiveNumber;
	ActiveNumber.Widget = Widget;
	ActiveNumber.WorldLocation = GetRandomizedLocation(HitLocation);
	ActiveNumber.Lifetime = Lifetime;
	ActiveNumber.RiseSpeed = RiseSpeed;

	UpdateActiveNumber(ActiveNumber);
	ActiveNumbers.Add(MoveTemp(ActiveNumber));
}

void US1CombatFeedbackSubsystem::ReleaseWidget(US1DamageNumberWidget* Widget)
{
	if (Widget == nullptr)
	{
		return;
	}

	Widget->ResetForPool();
	PooledWidgets.Add(Widget);
}

void US1CombatFeedbackSubsystem::UpdateActiveNumber(FS1ActiveFeedbackNumber& ActiveNumber) const
{
	if (ActiveNumber.Widget == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	if (PC == nullptr)
	{
		return;
	}

	const FVector WorldLocation = ActiveNumber.WorldLocation + FVector::UpVector * (ActiveNumber.RiseSpeed * ActiveNumber.ElapsedTime);

	FVector2D ScreenPosition;
	if (false == PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
	{
		ActiveNumber.Widget->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(ActiveNumber.Widget->Slot))
	{
		const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		Slot->SetPosition(ScreenPosition / FMath::Max(ViewportScale, KINDA_SMALL_NUMBER));
	}

	ActiveNumber.Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
}
