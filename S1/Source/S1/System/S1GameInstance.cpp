// Fill out your copyright notice in the Description page of Project Settings.

#include "S1GameInstance.h"
#include "S1AssetManager.h"
#include "S1HitLagManager.h"
#include "S1GameplayTags.h"
#include "S1LogChannels.h"

US1GameInstance::US1GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GameInstance::Init()
{
	Super::Init();

	US1AssetManager::Get().LoadAssetsToLabel(
		S1Labels::Label_Preload,
		FAsyncLabelLoadCompletedDelegate::CreateUObject(this, &ThisClass::OnPreloadComplete));
}

void US1GameInstance::OnPreloadComplete()
{
	LOG(TEXT("Preload Assets Complete"));

	if (US1HitLagManager* HitLag = GetSubsystem<US1HitLagManager>())
	{
		HitLag->BuildCache();
	}
}

void US1GameInstance::Shutdown()
{
	Super::Shutdown();
}
