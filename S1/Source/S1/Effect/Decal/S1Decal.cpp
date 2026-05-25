// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/Decal/S1Decal.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AS1Decal::AS1Decal()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(SceneRoot);
	DecalComponent->SetVisibility(false);
	DecalComponent->DecalSize = FVector(128.f, 128.f, 128.f);

	SetActorEnableCollision(false);
}

void AS1Decal::BeginPlay()
{
	Super::BeginPlay();

	HideDecal();
}

void AS1Decal::ShowDecal(const FVector& InLocation, const FRotator& InRotation, const FVector& InDecalSize)
{
	SetActorLocationAndRotation(InLocation, InRotation);

	if (DecalComponent)
	{
		DecalComponent->DecalSize = InDecalSize;
		DecalComponent->SetVisibility(true);
	}

	SetActorHiddenInGame(false);
	bIsActive = true;
}

void AS1Decal::HideDecal()
{
	if (DecalComponent)
	{
		DecalComponent->SetVisibility(false);
	}

	SetActorHiddenInGame(true);
	bIsActive = false;
}

bool AS1Decal::UpdateDecal(float DeltaTime)
{
	if (false == IsActive())
	{
		return true;
	}

	return false;
}

UMaterialInstanceDynamic* AS1Decal::CreateAndApplyDynamicMaterial()
{
	if (DynamicMaterial == nullptr)
	{
		if (DecalComponent && DecalComponent->GetDecalMaterial())
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(DecalComponent->GetDecalMaterial(), this);
		}
	}

	if (DecalComponent && DynamicMaterial)
	{
		DecalComponent->SetDecalMaterial(DynamicMaterial);
	}

	return DynamicMaterial;
}

