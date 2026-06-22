// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/S1NormalMonster.h"

#include "Component/S1MonsterHPBarComponent.h"

AS1NormalMonster::AS1NormalMonster()
	: Super()
{
	bUsePooling = true;

	MonsterHPBarComponent = CreateDefaultSubobject<US1MonsterHPBarComponent>(TEXT("MonsterHPBar"));
	if (MonsterHPBarComponent != nullptr)
	{
		MonsterHPBarComponent->SetupAttachment(GetMesh());
	}
}