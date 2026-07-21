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

	// 재트리거 시 튐 방지 — 이미 재생 중이거나 이전 값이 있으면 하드코딩된 0/1이 아니라 현재 값에서 이어서 시작
	float CurrentValue = bAppear ? 0.f : 1.f;
	if (DissolveMIDs.Num() > 0 && DissolveMIDs[0])
	{
		DissolveMIDs[0]->GetScalarParameterValue(DissolveParameterName, CurrentValue);
	}

	InitializeMIDs(TargetMesh);

	StartValue = CurrentValue;
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
