// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1UIData.generated.h"

class US1BaseWidget;

USTRUCT(BlueprintType)
struct FS1UserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag UITag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1BaseWidget> UserWidgetClass = nullptr;
};

UCLASS()
class S1_API US1UIData : public UDataAsset
{
	GENERATED_BODY()

public:
	const TSubclassOf<US1BaseWidget> FindUserWidgetClassByTag(const FGameplayTag& UITag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TArray<FS1UserWidget> UserWidgets;
};
