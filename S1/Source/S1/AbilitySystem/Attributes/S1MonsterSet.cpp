// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/S1MonsterSet.h"

#include "AbilitySystemComponent.h"
#include "Character/S1Monster.h"

void US1MonsterSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetHealthAttribute())
	{
		return;
	}

	if (GetHealth() > 0.f)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	if (AS1Monster* Monster = Cast<AS1Monster>(AbilitySystemComponent->GetAvatarActor()))
	{
		Monster->NotifyDeath();
	}
}
