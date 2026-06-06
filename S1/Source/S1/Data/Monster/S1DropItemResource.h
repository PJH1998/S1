// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1DropItemResource.generated.h"

class UMaterialInterface;
class UNiagaraSystem;
class UStaticMesh;
class UTexture2D;

UENUM(BlueprintType)
enum class ES1DropItemType : uint8
{
	Gold,
	Exp,
	Item
};

USTRUCT(BlueprintType)
struct FS1DropItemVisualParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor TintColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor EmissiveColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EmissiveIntensity = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Metallic = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Roughness = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> NoiseTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FlashIntensity = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NoiseScale = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FlashSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FlashWidth = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FlashSoftness = 0.02f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FlashContrast = 1.f;
};

USTRUCT(BlueprintType)
struct FS1DropItemResourceEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ES1DropItemType DropType = ES1DropItemType::Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag RarityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeshZOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Texture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> DropEffect;
};

UCLASS()
class S1_API US1DropItemResource : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FS1DropItemResourceEntry* FindResource(ES1DropItemType DropType, FGameplayTag ItemTag, FGameplayTag RarityTag) const;
	const FS1DropItemVisualParams& FindVisualParams(ES1DropItemType DropType, FGameplayTag RarityTag) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FS1DropItemResourceEntry> Resources;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FS1DropItemVisualParams DefaultVisual;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FS1DropItemVisualParams GoldVisual;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FS1DropItemVisualParams ExpVisual;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FS1DropItemVisualParams> RarityVisuals;
};
