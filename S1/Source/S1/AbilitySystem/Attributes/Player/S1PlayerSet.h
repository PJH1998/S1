// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "S1PlayerSet.generated.h"

UCLASS()
class S1_API US1PlayerSet : public US1AttributeSet
{
	GENERATED_BODY()

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName = NAME_None) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	void LevelUp();

public:
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentXP);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxXP);
	ATTRIBUTE_ACCESSORS(ThisClass, Level);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentXP = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxXP = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Level = 1.f;

	FGameplayTag CachedAssetTag;
	FGameplayTag CachedTableTag;
};
