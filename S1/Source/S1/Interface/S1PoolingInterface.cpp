// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/S1PoolingInterface.h"
#include "GameFramework/Actor.h"

void IS1PoolingInterface::OnSpawnFromPool(FVector Location, FRotator Rotation)
{
	if (AActor* Self = Cast<AActor>(_getUObject()))
	{
		Self->SetActorLocationAndRotation(Location, Rotation);
		Self->SetActorHiddenInGame(false);
		Self->SetActorEnableCollision(true);
		Self->SetActorTickEnabled(true);
	}
}

void IS1PoolingInterface::OnReturnToPool()
{
	if (AActor* Self = Cast<AActor>(_getUObject()))
	{
		Self->SetActorHiddenInGame(true);
		Self->SetActorEnableCollision(false);
		Self->SetActorTickEnabled(false);
	}
}
