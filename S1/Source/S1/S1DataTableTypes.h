// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Tags/S1GameplayTags.h"
#include "S1DataTableTypes.generated.h"

class USoundBase;

// -------------------------------------------------------
// 예제 Row 구조체
// -------------------------------------------------------
// USTRUCT(BlueprintType)
// struct FS1ExampleRow : public FTableRowBase
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SomeFloat = 0.f;
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 SomeInt = 0;
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText SomeText;
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag SomeTag;
// };
// -------------------------------------------------------

USTRUCT(BlueprintType)
struct FS1HitLagRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlowScale = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bExcludePlayer = true;
};

USTRUCT(BlueprintType)
struct FS1MonsterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStamina = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDefense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RewardXP = 0.f;
};

USTRUCT(BlueprintType)
struct FS1PlayerData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDefense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxXP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxUltimateGauge = 500.f;
};

USTRUCT(BlueprintType)
struct FS1ItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName = { TEXT("None") };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Rarity = { S1ItemRarityTags::Item_Rarity_Common };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EquipSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseHealth = { 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = { 7.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDefense = { 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Price = { 100.f };
};

// Footstep 트레이스로 얻은 EPhysicalSurface → 재생할 사운드 매핑. 행 이름은 임의(가독성용) — 조회는 SurfaceType 값으로 매칭.
USTRUCT(BlueprintType)
struct FS1FootstepSoundRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> SurfaceType = SurfaceType_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> FootstepSound;
};
