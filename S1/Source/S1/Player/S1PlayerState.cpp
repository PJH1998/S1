// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S1PlayerState.h"
#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Component/S1InventoryComponent.h"

AS1PlayerState::AS1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<US1PlayerAbilitySystemComponent>("AbilitySystemComponent");
	PlayerSet = CreateDefaultSubobject<US1PlayerSet>("PlayerSet");
	InventoryComponent = CreateDefaultSubobject<US1InventoryComponent>("InventoryComponent");

	//TEMP
	FS1DefaultAttribute DefaultAttribute;
	DefaultAttribute.MaxHealth = 100.f;
	DefaultAttribute.MaxStamina = 100.f;
	DefaultAttribute.BaseDamage = 10.f;
	DefaultAttribute.BaseDefense = 10.f;

	PlayerSet->InitDefaultAttribute(DefaultAttribute);
}

UAbilitySystemComponent* AS1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

US1AbilitySystemComponent* AS1PlayerState::GetS1AbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

US1PlayerSet* AS1PlayerState::GetS1PlayerSet() const
{
	return PlayerSet;
}

US1InventoryComponent* AS1PlayerState::GetInventoryComponent() const
{
	return InventoryComponent;
}

void AS1PlayerState::InitPlayerSetFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag)
{
	if (!::IsValid(PlayerSet)) { return; }
	PlayerSet->InitAttributeFromTable(AssetTag, TableTag);
}
