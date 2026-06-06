// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Monster/S1DropItemResource.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1ItemManager.generated.h"

class AController;
class AS1PlayerState;
struct FS1ItemData;

UCLASS()
class S1_API US1ItemManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	bool ApplyPickup(AController* OwnerController, ES1DropItemType DropType, FGameplayTag ItemTag, FGameplayTag RarityTag, int32 Amount);
	const FS1ItemData* FindItemData(FGameplayTag ItemTag) const;

private:
	bool ApplyExp(AS1PlayerState* PlayerState, int32 Amount) const;
	bool ApplyGold(AS1PlayerState* PlayerState, int32 Amount) const;
	bool ApplyItem(AS1PlayerState* PlayerState, FGameplayTag ItemTag, int32 Amount) const;
	AS1PlayerState* GetS1PlayerState(AController* OwnerController) const;
};
