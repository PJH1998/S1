// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "GameplayEffectExtension.h"

US1AttributeSet::US1AttributeSet()
{
}

void US1AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
}

void US1AttributeSet::InitDefaultAttribute(const FS1DefaultAttribute& DefaultAttribute)
{
	InitHealth(DefaultAttribute.MaxHealth);
	InitMaxHealth(DefaultAttribute.MaxHealth);
	InitStamina(DefaultAttribute.MaxStamina);
	InitMaxStamina(DefaultAttribute.MaxStamina);
	InitBaseDamage(DefaultAttribute.BaseDamage);
	InitBaseDefense(DefaultAttribute.BaseDefense);
}
