// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "S1EquipComponent.generated.h"

class UGameplayEffect;

struct FS1ItemData;

UENUM(BlueprintType)
enum class ES1EquipSlot : uint8
{
	Weapon,
	Costume,
	Accessary,
	MAX UMETA(Hidden)
};

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1ItemEquippedSignature, FGameplayTag, ItemTag);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1EquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1EquipComponent();

protected:
	virtual void BeginPlay() override;

public:
	bool EquipItem(FGameplayTag ItemTag);
	bool UnequipItem(FGameplayTag SlotTag, bool bFromEquipSwap = false);
	FGameplayTag GetEquippedItemTag(FGameplayTag SlotTag) const;
	const TArray<FS1EquippedItem>& GetEquippedItems() const { return EquippedItems; }

	UPROPERTY(BlueprintAssignable)
	FS1EquipmentChangedSignature OnEquipmentChanged;

	UPROPERTY(BlueprintAssignable)
	FS1ItemEquippedSignature OnItemEquipped;

private:
	TOptional<ES1EquipSlot> GetEquipSlotEnum(FGameplayTag SlotTag) const;
	FGameplayTag GetEquipSlotTag(ES1EquipSlot Slot) const;
	bool ApplyEquipGameplayEffect(ES1EquipSlot Slot, const FS1ItemData& ItemData);
	void RemoveEquipGameplayEffect(ES1EquipSlot Slot);

private:
	static constexpr int32 EquipSlotCount = static_cast<int32>(ES1EquipSlot::MAX);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FS1EquippedItem> EquippedItems;

	TArray<FActiveGameplayEffectHandle> EquipEffectHandles;

	TSubclassOf<UGameplayEffect> ApplyEquipEffectClass;
};
