// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1BaseWidget.generated.h"

/**
 *
 */
UCLASS()
class S1_API US1BaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// BaseTag에 성별 접미사(.Male/.Female)를 붙인 GameplayTag 반환 — 성별별 리소스(아이콘 등) 조회용
	static FGameplayTag AppendGenderSuffix(const FGameplayTag& BaseTag, EPlayerGender Gender);
};
