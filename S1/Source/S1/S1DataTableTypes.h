// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "S1DataTableTypes.generated.h"

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
 struct FS1PlayerData : public FTableRowBase
 {
 	GENERATED_BODY()

 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 10.f;
 };
