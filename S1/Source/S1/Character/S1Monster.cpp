// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Monster.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AI/S1AIController.h"

AS1Monster::AS1Monster()
	: Super()
{
	AbilitySystemComponent = CreateDefaultSubobject<US1AbilitySystemComponent>("AbilitySystemComponent");
}

void AS1Monster::BeginPlay()
{
	Super::BeginPlay();

	InitSystem();
}

void AS1Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS1Monster::InitSystem()
{
	// Ability
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
