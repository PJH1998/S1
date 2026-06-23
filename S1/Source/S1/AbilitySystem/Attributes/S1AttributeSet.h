// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "S1AttributeSet.generated.h"

class FLifetimeProperty;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)			\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FS1DefaultAttribute
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxStamina = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDefense = 0.f;
};

UCLASS()
class S1_API US1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:	
	US1AttributeSet();

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Stamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDefense);

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitDefaultAttribute(const FS1DefaultAttribute& DefaultAttribute);
	virtual void InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName = NAME_None) {}

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage);

	UFUNCTION()
	void OnRep_BaseDefense(const FGameplayAttributeData& OldBaseDefense);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Stamina = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxStamina = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDamage = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDefense, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDefense = 0.f;
};
