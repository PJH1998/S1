// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/S1Effect.h"

// Sets default values
AS1Effect::AS1Effect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AS1Effect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AS1Effect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

