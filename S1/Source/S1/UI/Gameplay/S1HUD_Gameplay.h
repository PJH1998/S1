// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1HUD_Gameplay.generated.h"

class AS1BossBase;
class AS1Monster;
class US1BossStatus;
class US1InteractComponent;
class UWidget;
class UWidgetAnimation;
class US1RespawnPrompt;
/**
 *
 */
UCLASS()
class S1_API US1HUD_Gameplay : public US1BaseWidget
{
	GENERATED_BODY()

public:
	US1HUD_Gameplay(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 리스폰으로 Pawn이 바뀌면 GetOwningPlayerPawn()도 새 Pawn을 가리키게 되지만, NativeConstruct 때 1회
	// 바인딩해둔 델리게이트는 옛(파괴된) Pawn의 InteractComponent를 그대로 구독 중이라 새 Pawn 걸로
	// 다시 불러줘야 함(AS1PlayerController::AcknowledgePossession에서 호출) — 재호출해도 안전(기존 바인딩 해제 후 재바인딩).
	void BindInteractEvents();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void BindBossEvents();
	void SetUpBossStatus();

	void ShowBossUI(AS1BossBase* InBoss);
	void HideBossUI(AS1BossBase* InBoss);

	void SetUpInteractPrompt();

	void ShowInteractPrompt();
	void HideInteractPrompt();

	void BindRespawnEvents();
	void SetUpRespawnPrompt();

	void ShowRespawnPrompt();
	void HideRespawnPrompt();

private:
	UFUNCTION()
	void HandleBossHasTargetChanged(AS1Monster* InMonster, bool bInHasTarget);

	UFUNCTION()
	void HandleHideAnimationFinished();

	UFUNCTION()
	void HandleNearestInteractableChanged(AActor* NewNearest);

	UFUNCTION()
	void HandleInteractPromptHideAnimationFinished();

	UFUNCTION()
	void HandleRespawnPromptHideAnimationFinished();

	// ASC->RegisterGameplayTagEvent 콜백 — State.Dead.CanRespawn 부여/해제 시 호출
	void HandleCanRespawnTagChanged(const FGameplayTag Tag, int32 NewCount);

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

	// State.Dead.CanRespawn 부여 시 노출되는 리스폰 프롬프트(Button+이미지) — 클릭 처리는 US1RespawnPrompt
	// 자신이 하므로, 여기(부모)서는 Show/Hide+Animation만 담당.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1RespawnPrompt> RespawnPrompt;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_RespawnPrompt_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_RespawnPrompt_FadeOut;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1BossBase>> BoundBosses;

	UPROPERTY(Transient)
	TObjectPtr<AS1BossBase> CurrentBoss;

	UPROPERTY(Transient)
	TWeakObjectPtr<US1InteractComponent> BoundInteractComponent;

	// ASC->RegisterGameplayTagEvent 구독 해제용
	FDelegateHandle CanRespawnTagEventHandle;
};
