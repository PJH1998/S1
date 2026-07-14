// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1HUD_Gameplay.generated.h"

class AS1BossBase;
class AS1Monster;
class AS1PuzzleButton_Gimmick;
class US1BossStatus;
class US1InteractComponent;
class UWidget;
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

	void BindInteractEvents();
	void SetUpInteractPrompt();

	void ShowInteractPrompt();
	void HideInteractPrompt();

private:
	UFUNCTION()
	void HandleBossHasTargetChanged(AS1Monster* InMonster, bool bInHasTarget);

	UFUNCTION()
	void HandleHideAnimationFinished();

	UFUNCTION()
	void HandleNearestInteractableChanged(AS1PuzzleButton_Gimmick* NewNearest);

	UFUNCTION()
	void HandleInteractPromptHideAnimationFinished();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1BossStatus> BossStatus;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_BossStatus_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_BossStatus_FadeOut;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> InteractPrompt;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_InteractPrompt_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_InteractPrompt_FadeOut;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1BossBase>> BoundBosses;

	UPROPERTY(Transient)
	TObjectPtr<AS1BossBase> CurrentBoss;

	UPROPERTY(Transient)
	TWeakObjectPtr<US1InteractComponent> BoundInteractComponent;
};
