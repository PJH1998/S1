// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

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

void US1AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(US1AttributeSet, BaseDefense, COND_None, REPNOTIFY_Always);
}

void US1AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, Health, OldHealth);
}

void US1AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, MaxHealth, OldMaxHealth);
}

void US1AttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, Stamina, OldStamina);
}

void US1AttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, MaxStamina, OldMaxStamina);
}

void US1AttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, BaseDamage, OldBaseDamage);
}

void US1AttributeSet::OnRep_BaseDefense(const FGameplayAttributeData& OldBaseDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(US1AttributeSet, BaseDefense, OldBaseDefense);
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
