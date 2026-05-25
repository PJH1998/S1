// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1HUD_Gameplay.generated.h"

class AS1BossBase;
class US1BossStatus;
class UWidgetAnimation;
/**
 * 
 */
UCLASS()
class S1_API US1HUD_Gameplay : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1HUD_Gameplay(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void BindBossEvents();
	void SetUpBossStatus();

	void ShowBossUI(AS1BossBase* InBoss);
	void HideBossUI(AS1BossBase* InBoss);

private:
	UFUNCTION()
	void HandleBossHasTargetChanged(AS1BossBase* InBoss, bool bInHasTarget);

	UFUNCTION()
	void HandleHideAnimationFinished();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1BossStatus> BossStatus;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_BossStatus_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_BossStatus_FadeOut;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1BossBase>> BoundBosses;

	UPROPERTY(Transient)
	TObjectPtr<AS1BossBase> CurrentBoss;
};
