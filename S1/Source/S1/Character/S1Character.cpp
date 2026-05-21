// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Character.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"

// Sets default values
AS1Character::AS1Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AS1Character::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AS1Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AS1Character::InitSystem()
{
}
