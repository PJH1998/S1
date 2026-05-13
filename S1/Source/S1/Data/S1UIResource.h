// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1UIResource.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FS1UITexture
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ResourceTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Texture = nullptr;
};

UCLASS()
class S1_API US1UIResource : public UDataAsset
{
	GENERATED_BODY()

public:
	UTexture2D* FindTextureByTag(const FGameplayTag& ResourceTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TArray<FS1UITexture> Resources;
};
