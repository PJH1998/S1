// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// 클라 UI 진입점 — 서버 권한이면 즉시 처리, 아니면 서버 RPC로 위임
	void RequestEquipItem(FGameplayTag ItemTag);
	void RequestUnequipItem(FGameplayTag SlotTag);

	FGameplayTag GetEquippedItemTag(FGameplayTag SlotTag) const;
	const TArray<FS1EquippedItem>& GetEquippedItems() const { return EquippedItems; }

	UPROPERTY(BlueprintAssignable)
	FS1EquipmentChangedSignature OnEquipmentChanged;

	UPROPERTY(BlueprintAssignable)
	FS1ItemEquippedSignature OnItemEquipped;

private:
	// 서버 권한에서 실제 장착/해제를 수행하는 처리기 (RequestEquipItem/RequestUnequipItem 경유)
	bool EquipItem(FGameplayTag ItemTag);
	bool UnequipItem(FGameplayTag SlotTag, bool bFromEquipSwap = false);

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(FGameplayTag ItemTag);

	UFUNCTION(Server, Reliable)
	void ServerUnequipItem(FGameplayTag SlotTag);

	UFUNCTION()
	void OnRep_EquippedItems();

	TOptional<ES1EquipSlot> GetEquipSlotEnum(FGameplayTag SlotTag) const;
	FGameplayTag GetEquipSlotTag(ES1EquipSlot Slot) const;
	bool ApplyEquipGameplayEffect(ES1EquipSlot Slot, const FS1ItemData& ItemData);
	void RemoveEquipGameplayEffect(ES1EquipSlot Slot);

private:
	static constexpr int32 EquipSlotCount = static_cast<int32>(ES1EquipSlot::MAX);

	UPROPERTY(ReplicatedUsing = OnRep_EquippedItems, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FS1EquippedItem> EquippedItems;

	TArray<FActiveGameplayEffectHandle> EquipEffectHandles;

	TSubclassOf<UGameplayEffect> ApplyEquipEffectClass;
};
