// Fill out your copyright notice in the Description page of Project Settings.

#include "S1GameInstance.h"
#include "S1AssetManager.h"
#include "S1HitLagManager.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"
#include "Containers/Ticker.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/SOverlay.h"

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

	// 첫 레벨이 로드되어 렌더링을 시작한 직후~Lobby Root UI가 뜨기 전 사이의 몇 프레임 동안 맨 레벨이 노출되는 걸 막음
	// (AS1PlayerController::BeginPlay가 Lobby Root UI를 만든 직후 HideBootstrapOverlay 호출)
	ShowBootstrapOverlay();
}

void US1GameInstance::ShowBootstrapOverlay()
{
	// HideBootstrapOverlay가 이미 호출된 뒤라면(비정상적인 호출 순서 등) 다시 띄우지 않음 — 안 그러면 이미 뜬 UI를 뒤늦게 덮어서 영구히 안 사라지는 화면이 됨
	if (bBootstrapOverlayHidden)
	{
		return;
	}

	if (TryCreateBootstrapOverlay())
	{
		return;
	}

	// 뷰포트가 아직 준비 안 됐음 — 매 틱 재시도. 성공하거나 그 사이 Hide가 먼저 호출되면 중단(위 플래그로 안전 보장)
	TWeakObjectPtr<US1GameInstance> WeakThis(this);
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float) -> bool
	{
		US1GameInstance* This = WeakThis.Get();
		if (nullptr == This || This->bBootstrapOverlayHidden)
		{
			return false;
		}
		return false == This->TryCreateBootstrapOverlay();
	}));
}

bool US1GameInstance::TryCreateBootstrapOverlay()
{
	UGameViewportClient* Viewport = GetGameViewportClient();
	if (nullptr == Viewport)
	{
		return false;
	}

	LOG(TEXT("GameInstance: ShowBootstrapOverlay — added"));

	// SColorBlock 단독으론 자기 DesiredSize(작은 고정값)만큼만 그려질 수 있어 Fill 정렬 Overlay로 감싸서 확실히 풀스크린 보장
	BootstrapOverlay = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SColorBlock).Color(FLinearColor::Black)
		];

	Viewport->AddViewportWidgetContent(BootstrapOverlay.ToSharedRef(), TNumericLimits<int32>::Max());
	return true;
}

void US1GameInstance::HideBootstrapOverlay()
{
	bBootstrapOverlayHidden = true;

	if (false == BootstrapOverlay.IsValid())
	{
		LOG(TEXT("GameInstance: HideBootstrapOverlay — already hidden/never shown"));
		return;
	}

	LOG(TEXT("GameInstance: HideBootstrapOverlay — removed"));

	if (UGameViewportClient* Viewport = GetGameViewportClient())
	{
		Viewport->RemoveViewportWidgetContent(BootstrapOverlay.ToSharedRef());
	}
	BootstrapOverlay.Reset();
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
