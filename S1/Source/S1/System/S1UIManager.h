// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "S1UIManager.generated.h"

#define SUBSYSTEM(T) Cast<T>(USubsystemBlueprintLibrary::GetLocalPlayerSubsystem(this, T::StaticClass()))

class US1RootWidget;
/**
 *
 */

UCLASS()
class S1_API US1UIManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	US1UIManager();

public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

public:
	void Create_RootUI(APlayerController* PC, const FGameplayTag& RootUITag);
	void UnRegister_RootUI();

public:
	void FadeIn(float InDuration = 1.f, TFunction<void()> InCallback = nullptr);
	void FadeOut(float InDuration = 1.f, TFunction<void()> InCallback = nullptr);

	US1RootWidget* GetRootWidget() const { return RootWidget; }

	// 상황별 텍스처 어나운스(퀘스트/기믹/보스 클리어 등) 표시. BP 이벤트 그래프에서 바로 호출할 수 있도록
	// BlueprintCallable로 노출한다.
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowAnnounce(const FGameplayTag& AnnounceTag);

private:
	UFUNCTION()
	void FinishedFade();

private:
	TObjectPtr<US1RootWidget> RootWidget;

private:
	TFunction<void()> FadeEndCallback;

private:
	bool bOnFade = { false };
};
