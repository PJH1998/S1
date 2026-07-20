// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1RespawnPrompt.generated.h"

class UButton;

// InteractPrompt류와 달리 Button+이미지 여러 개로 구성된 리스폰 프롬프트 — 자기 Button 클릭을 직접 처리해서
// US1HUD_Gameplay(부모)는 이 위젯을 그냥 보이기/숨기기+애니메이션만 하면 된다.
UCLASS()
class S1_API US1RespawnPrompt : public US1BaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleRespawnButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Respawn;
};
