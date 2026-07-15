// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Announce.generated.h"

class UImage;
class UWidgetAnimation;

/**
 * 상황별(퀘스트 완료/기믹 완료/보스 클리어 등) 텍스처 한 장을 페이드인→일정 시간 후 페이드아웃으로
 * 보여주는 범용 어나운스 위젯. US1UIManager::ShowAnnounce(FGameplayTag)를 통해 호출된다.
 */
UCLASS()
class S1_API US1Announce : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1Announce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// AnnounceTag로 US1UIResource에서 텍스처를 조회해 표시한다. 못 찾으면 아무 것도 하지 않는다.
	void ShowAnnounce(const FGameplayTag& AnnounceTag);
	void Hide();

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleHideAnimationFinished();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Announce;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeOut;

	UPROPERTY(EditDefaultsOnly, Category = "Announce")
	float DisplayDuration = 2.5f;

private:
	FTimerHandle HideTimerHandle;
};
