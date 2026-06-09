// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "S1EquipComponent.generated.h"

USTRUCT(BlueprintType)
struct FS1EquippedItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotTag;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1EquipmentChangedSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1EquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1EquipComponent();

public:
	bool EquipItem(FGameplayTag ItemTag);
	bool UnequipItem(FGameplayTag SlotTag);
	FGameplayTag GetEquippedItemTag(FGameplayTag SlotTag) const;
	const TArray<FS1EquippedItem>& GetEquippedItems() const { return EquippedItems; }

	UPROPERTY(BlueprintAssignable)
	FS1EquipmentChangedSignature OnEquipmentChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FS1EquippedItem> EquippedItems;
};
