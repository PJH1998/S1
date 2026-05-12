// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/S1AttributeSet.h"

US1AttributeSet::US1AttributeSet()
{
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
