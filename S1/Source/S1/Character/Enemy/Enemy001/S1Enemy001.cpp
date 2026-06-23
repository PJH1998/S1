// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Enemy/Enemy001/S1Enemy001.h"

#include "AI/S1AIController.h"
#include "AbilitySystem/Attributes/S1MonsterSet.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Component/S1DeathPresentationComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/S1GameplayTags.h"

AS1Enemy001::AS1Enemy001()
	: Super()
{
	AIControllerClass = AS1AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
	GetCapsuleComponent()->SetCapsuleRadius(42.f);

	bUseControllerRotationYaw = false;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = false;
	Movement->RotationRate.Yaw = 180.f;
	Movement->MaxWalkSpeed = DefaultMaxWalkSpeed;
	Movement->MaxAcceleration = DefaultMaxAcceleration;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -96.f), FRotator(0.f, -90.f, 0.f));

	AttributeSet = CreateDefaultSubobject<US1MonsterSet>("AttributeSet");

	DropTableTag = S1AssetTags::Asset_DropTable_Enemy001;

	if (DeathPresentationComponent)
	{
		DeathPresentationComponent->PresentationMode = EDeathPresentation::AlphaFade;
		DeathPresentationComponent->FadeDuration = 2.f;
	}
}

void AS1Enemy001::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->InitAttributeFromTable(S1AssetTags::Asset_DataTable, S1DataTableTags::DataTable_MonsterData, TEXT("Enemy001"));

	if (HasAuthority())
	{
		AbilitySystemComponent->AddCharacterAbilities(S1AssetTags::Asset_Ability_Enemy001);
	}
}
