// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "S1InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FS1InventoryItemStack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag;

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1GoldChangedSignature, int32, NewGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1InventoryChangedSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	US1InventoryComponent();

public:
	bool AddGold(int32 Amount);
	bool AddItem(FGameplayTag ItemTag, int32 Count);

	int32 GetGold() const { return Gold; }
	const TArray<FS1InventoryItemStack>& GetItemStacks() const { return ItemStacks; }
	int32 GetItemCount(FGameplayTag ItemTag) const;

	UPROPERTY(BlueprintAssignable)
	FS1GoldChangedSignature OnGoldChanged;

	UPROPERTY(BlueprintAssignable)
	FS1InventoryChangedSignature OnInventoryChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Gold = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FS1InventoryItemStack> ItemStacks;
};
