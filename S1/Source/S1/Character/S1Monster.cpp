// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Monster.h"

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AI/S1AIController.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void AS1Monster::PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void AS1Monster::NotifyDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->CancelAllAbilities();
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();

		if (AS1AIController* S1AIController = Cast<AS1AIController>(AIController))
		{
			S1AIController->SetBlackboardIsDead(true);
		}
	}
}

void AS1Monster::HandleDeath()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AS1Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS1Monster::InitSystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
