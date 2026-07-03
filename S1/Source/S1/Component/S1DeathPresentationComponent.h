// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1DeathPresentationComponent.generated.h"

class UMaterialInstanceDynamic;
class UTexture2D;

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

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	TObjectPtr<UTexture2D> DissolveTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	FName DissolveTextureParameterName = TEXT("DissolveTexture");

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	FName DissolveAmountParameterName = TEXT("DissolveAmount");

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	FLinearColor DissolveEdgeColor = FLinearColor(10.f, 2.f, 0.5f);

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	FName DissolveEdgeColorParameterName = TEXT("DissolveEdgeColor");

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides, ClampMin = "0.0"))
	float DissolveEdgeWidth = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (EditCondition = "PresentationMode == EDeathPresentation::Dissolve", EditConditionHides))
	FName DissolveEdgeWidthParameterName = TEXT("DissolveEdgeWidth");

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CompletePresentation();
	void ClearPresentationTimer();
	void InitializeFadeMaterials();
	void ApplyFadeAlpha(float Alpha);
	void ApplyDissolveAmount(float Amount);
	void ResetFadeMaterials();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> FadeMIDs;

	FTimerHandle PresentationTimerHandle;
	float PresentationElapsed = 0.f;
	bool bIsPresenting = false;
};
