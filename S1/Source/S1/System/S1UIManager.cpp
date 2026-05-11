// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1UIManager.h"
#include "UI/S1RootWidget.h"
#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1UIData.h"

US1UIManager::US1UIManager()
	: Super()
{
	//ConstructorHelpers::FClassFinder<US1RootWidget> FindRoot(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_Root.WBP_Root_C'"));
	//if (FindRoot.Succeeded())
	//{
	//	RootWidgetClass = FindRoot.Class;
	//}
}

void US1UIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void US1UIManager::Deinitialize()
{
	Super::Deinitialize();
}

void US1UIManager::Create_RootUI(APlayerController* PC)
{
	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1DataTags::Data_Input, S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> RootWidgetClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Root))
		{
			RootWidget = CreateWidget<US1RootWidget>(PC, RootWidgetClass);
		}
	}

	if (RootWidget == nullptr)
	{
		UE_LOG(LogWindows, Error, TEXT("Failed to Create : RootWidget"));
		return;
	}
	RootWidget->SetUp_HUD(S1UITags::UI_Lobby_HUD);

	RootWidget->AddToViewport();

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
