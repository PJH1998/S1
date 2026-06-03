// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "NiagaraFunctionLibrary.h"

void AS1NiagaraEffect::PlayEffect(UWorld* World, FVector Location, FRotator Rotation) const
{
	if (nullptr == World || nullptr == NiagaraSystem)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		Location,
		Rotation,
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::AutoRelease);
}
