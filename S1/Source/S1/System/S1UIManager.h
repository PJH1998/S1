// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void Create_RootUI(APlayerController* PC);
	void UnRegister_RootUI();

public:
	void FadeIn(float InDuration = 1.f, TFunction<void()> InCallback = nullptr);
	void FadeOut(float InDuration = 1.f, TFunction<void()> InCallback = nullptr);

	US1RootWidget* GetRootWidget() const { return RootWidget; }

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
