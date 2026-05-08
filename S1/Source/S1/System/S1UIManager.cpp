// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1UIManager.h"
#include "UI/S1RootWidget.h"

void US1UIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void US1UIManager::Deinitialize()
{
	Super::Deinitialize();
}

void US1UIManager::Register_RootUI(US1RootWidget* InRootWidget)
{
	if (InRootWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("RootUI is not exist."));
		return;
	}

	RootWidget = InRootWidget;
	RootWidget->DeliverFinished.RemoveDynamic(this, &ThisClass::FinishedFade);
	RootWidget->DeliverFinished.AddDynamic(this, &ThisClass::FinishedFade);
}

void US1UIManager::UnRegister_RootUI()
{
	if (RootWidget != nullptr)
	{
		RootWidget->DeliverFinished.RemoveDynamic(this, &ThisClass::FinishedFade);
	}

	RootWidget = nullptr;
	FadeEndCallback = nullptr;
	bOnFade = false;
}

void US1UIManager::FadeIn(float InDuration, TFunction<void()> InCallback)
{
	if (RootWidget == nullptr || bOnFade)
	{
		return;
	}

	FadeEndCallback = MoveTemp(InCallback);
	bOnFade = true;
	RootWidget->FadeIn(InDuration);
}

void US1UIManager::FadeOut(float InDuration, TFunction<void()> InCallback)
{
	if (RootWidget == nullptr || bOnFade)
	{
		return;
	}

	FadeEndCallback = MoveTemp(InCallback);
	bOnFade = true;
	RootWidget->FadeOut(InDuration);
}

void US1UIManager::FinishedFade()
{
	if (RootWidget == nullptr)
	{
		return;
	}

	bOnFade = false;

	TFunction<void()> Callback = MoveTemp(FadeEndCallback);
	FadeEndCallback = nullptr;

	if (Callback)
	{
		Callback();
	}
}
