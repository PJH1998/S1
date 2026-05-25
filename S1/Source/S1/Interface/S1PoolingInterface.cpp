// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/S1PoolingInterface.h"
#include "System/S1PoolingManager.h"
#include "GameFramework/Actor.h"

void IS1PoolingInterface::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	PoolTag = InPoolTag;

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

void IS1PoolingInterface::ReturnSelf()
{
	AActor* Self = Cast<AActor>(_getUObject());
	if (false == IsValid(Self))
	{
		return;
	}

	US1PoolingManager* PoolingManager = Self->GetWorld()->GetSubsystem<US1PoolingManager>();
	if (false == IsValid(PoolingManager))
	{
		return;
	}

	PoolingManager->ReturnToPool(Self, PoolTag);
}
