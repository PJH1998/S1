// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/NiagaraEffect/S1DamageNumberEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UNiagaraComponent* AS1DamageNumberEffect::SpawnDigitComponent(UWorld* World, int32 DigitIndex) const
{
	if (World == nullptr || NiagaraSystem == nullptr)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		false,
		ENCPoolMethod::AutoRelease);

	if (NiagaraComponent == nullptr)
	{
		return nullptr;
	}

	NiagaraComponent->SetVariableFloat(DigitIndexParameterName, static_cast<float>(DigitIndex));
	NiagaraComponent->Activate(true);
	return NiagaraComponent;
}
