// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1DeathPresentationComponent.generated.h"

class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathPresentationComplete);

UENUM(BlueprintType)
enum class EDeathPresentation : uint8
{
	None,
	AlphaFade,
	Dissolve,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1DeathPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1DeathPresentationComponent();

	UFUNCTION(BlueprintCallable)
	void StartPresentation();

	UFUNCTION(BlueprintCallable)
	void StopPresentation();

	UPROPERTY(BlueprintAssignable)
	FOnDeathPresentationComplete OnPresentationComplete;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	EDeathPresentation PresentationMode = EDeathPresentation::AlphaFade;

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (ClampMin = "0.0"))
	float FadeDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	FName OpacityParameterName = TEXT("Opacity");

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CompletePresentation();
	void ClearPresentationTimer();
	void InitializeFadeMaterials();
	void ApplyFadeAlpha(float Alpha);
	void ResetFadeMaterials();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> FadeMIDs;

	FTimerHandle PresentationTimerHandle;
	float PresentationElapsed = 0.f;
	bool bIsPresenting = false;
};
