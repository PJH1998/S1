// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1CharacterSelectData.generated.h"

class AS1Player;

UCLASS()
class S1_API US1CharacterSelectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UClass* GetCharacterClass(const FGameplayTag& Tag) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TMap<FGameplayTag, TSubclassOf<AS1Player>> CharacterClasses;
};
