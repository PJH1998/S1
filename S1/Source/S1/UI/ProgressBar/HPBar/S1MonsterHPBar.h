// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBar/S1ProgressBar.h"
#include "S1MonsterHPBar.generated.h"

class AS1Monster;
class UTextBlock;
class UWidgetAnimation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMonsterHPBarHideFinishedDelegate);

UCLASS()
class S1_API US1MonsterHPBar : public US1ProgressBar
{
	GENERATED_BODY()

public:
	US1MonsterHPBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetMonster(AS1Monster* InMonster);
	void ShowBar();
	void HideBar();
	void ForceHide();

	UPROPERTY(BlueprintAssignable)
	FMonsterHPBarHideFinishedDelegate OnHideFinished;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	virtual void Bind_ShaderResource() override;

private:
	UFUNCTION()
	void HandleHideAnimationFinished();

private:
	float LerpValue = { 100.f };
	float PrevHealthForDamageNumber = -1.f;

	FName LerpRatioParameterName = TEXT("LerpRatio");

	UPROPERTY(Transient)
	TObjectPtr<AS1Monster> Monster;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeOut;
};
