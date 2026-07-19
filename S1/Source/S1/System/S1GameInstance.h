// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "S1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	US1GameInstance(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	// 레벨이 로드되어 렌더링을 시작한 뒤 첫 UI(Lobby Root)가 뜨기 전까지의 프레임 동안 맨 레벨이 노출되는 걸 막기 위한 부트스트랩 오버레이 해제
	void HideBootstrapOverlay();

private:
	void OnPreloadComplete();

	void ShowBootstrapOverlay();
	// 뷰포트가 준비됐으면 오버레이를 만들고 true, 아직이면 false
	bool TryCreateBootstrapOverlay();

private:
	TSharedPtr<class SWidget> BootstrapOverlay;

	// HideBootstrapOverlay가 먼저 호출된 뒤 ShowBootstrapOverlay가 뒤늦게 실행되어 이미 뜬 UI를 다시 덮는 레이스 방지
	bool bBootstrapOverlayHidden = false;
};
