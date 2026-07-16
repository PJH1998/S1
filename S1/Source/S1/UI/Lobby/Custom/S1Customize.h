// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Customize.generated.h"

class UImage;
class UButton;

/**
 *
 */
UCLASS()
class S1_API US1Customize : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1Customize(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnStartClicked();

	// Pawn::BeginPlay(인스턴스 렌더타깃 생성)가 Controller::BeginPlay(이 위젯 생성)보다 늦게 실행될 수 있어
	// NativeConstruct 시점엔 아직 인스턴스가 없을 수 있음 — 1틱 뒤 재시도로 확실히 최신 RT를 바인딩
	void BindPreviewRenderTarget();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CharacterModel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Start;
};
