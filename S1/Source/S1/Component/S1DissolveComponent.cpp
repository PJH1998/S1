// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1DissolveComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

US1DissolveComponent::US1DissolveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void US1DissolveComponent::PlayDissolve(UMeshComponent* TargetMesh, bool bAppear, float Duration)
{
	if (false == IsValid(TargetMesh) || Duration <= 0.f)
	{
		return;
	}

	InitializeMIDs(TargetMesh);

	StartValue = bAppear ? 0.f : 1.f;
	EndValue   = bAppear ? 1.f : 0.f;
	PlayDuration = Duration;
	ElapsedTime = 0.f;
	bPlaying = true;

	ApplyDissolveAmount(StartValue);
	SetComponentTickEnabled(true);
}

void US1DissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (false == bPlaying)
	{
		return;
	}

	ElapsedTime += DeltaTime;
	const float Progress = FMath::Clamp(ElapsedTime / PlayDuration, 0.f, 1.f);
	ApplyDissolveAmount(FMath::Lerp(StartValue, EndValue, Progress));

	if (ElapsedTime >= PlayDuration)
	{
		bPlaying = false;
		SetComponentTickEnabled(false);
		OnDissolveComplete.Broadcast();
	}
}

void US1DissolveComponent::InitializeMIDs(UMeshComponent* TargetMesh)
{
	DissolveMIDs.Empty();

	const int32 MaterialCount = TargetMesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		if (UMaterialInstanceDynamic* MID = TargetMesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex))
		{
			DissolveMIDs.Add(MID);
		}
	}
}

void US1DissolveComponent::ApplyDissolveAmount(float Amount)
{
	const float ClampedAmount = FMath::Clamp(Amount, 0.f, 1.f);
	for (UMaterialInstanceDynamic* MID : DissolveMIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(DissolveParameterName, ClampedAmount);
		}
	}
}
