// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/Boss000/S1Boss_000.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "S1GameplayTags.h"

#include "Components/CapsuleComponent.h"

AS1Boss_000::AS1Boss_000()
	: Super()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(400.f);
	GetCapsuleComponent()->SetCapsuleRadius(200.f);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -400.f), FRotator(0.f, -90.f, 0.f));

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

	AbilitySystemComponent->AddCharacterAbilities(S1AssetTags::Asset_Ability);
}

void AS1Boss_000::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
