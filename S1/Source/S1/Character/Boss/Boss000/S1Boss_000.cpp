// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/Boss000/S1Boss_000.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "S1GameplayTags.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AS1Boss_000::AS1Boss_000()
	: Super()
{
	// Collider
	GetCapsuleComponent()->SetCapsuleHalfHeight(400.f);
	GetCapsuleComponent()->SetCapsuleRadius(200.f);

	// Movement — Chase Notify가 yaw를 제어하므로 자동 회전은 비활성
	bUseControllerRotationYaw = false;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = false;
	Movement->RotationRate.Yaw = 180.f;
	Movement->MaxWalkSpeed = DefaultMaxWalkSpeed;
	Movement->MaxAcceleration = DefaultMaxAcceleration;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -400.f), FRotator(0.f, -90.f, 0.f));

	// AttributeSet
	AttributeSet = CreateDefaultSubobject<US1BossSet>("AttributeSet");

	//FS1DefaultAttribute DefaultAttribute;
	//DefaultAttribute.MaxHealth = 100.f;
	//DefaultAttribute.MaxStamina = 100.f;
	//DefaultAttribute.BaseDamage = 10.f;
	//DefaultAttribute.BaseDefense = 10.f;
	//AttributeSet->InitDefaultAttribute(DefaultAttribute);

	// UI
	BossName = TEXT("Cobalt");
}

void AS1Boss_000::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->InitAttributeFromTable(S1AssetTags::Asset_DataTable, S1DataTableTags::DataTable_MonsterData, TEXT("Boss000"));

	AbilitySystemComponent->AddCharacterAbilities(S1AssetTags::Asset_Ability_Boss000);
}

void AS1Boss_000::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
