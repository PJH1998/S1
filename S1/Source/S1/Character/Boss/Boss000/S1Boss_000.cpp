// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/Boss000/S1Boss_000.h"

#include "AbilitySystem/Attributes/S1BossSet.h"

#include "Components/CapsuleComponent.h"

AS1Boss_000::AS1Boss_000()
	: Super()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(400.f);
	GetCapsuleComponent()->SetCapsuleRadius(200.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -400.f));

	AttributeSet = CreateDefaultSubobject<US1BossSet>("AttributeSet");

	FS1DefaultAttribute DefaultAttribute;
	DefaultAttribute.MaxHealth = 100.f;
	DefaultAttribute.MaxStamina = 100.f;
	DefaultAttribute.BaseDamage = 10.f;
	DefaultAttribute.BaseDefense = 10.f;
	AttributeSet->InitDefaultAttribute(DefaultAttribute);
}

void AS1Boss_000::BeginPlay()
{
	Super::BeginPlay();
}

void AS1Boss_000::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
