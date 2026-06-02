// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1DeathPresentationComponent.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

US1DeathPresentationComponent::US1DeathPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void US1DeathPresentationComponent::StartPresentation()
{
	StopPresentation();

	switch (PresentationMode)
	{
	case EDeathPresentation::None:
		CompletePresentation();
		break;
	case EDeathPresentation::AlphaFade:
	case EDeathPresentation::Dissolve:
		if (FadeDuration <= 0.f)
		{
			CompletePresentation();
			break;
		}

		bIsPresenting = true;
		PresentationElapsed = 0.f;
		InitializeFadeMaterials();
		SetComponentTickEnabled(true);
		break;
	default:
		CompletePresentation();
		break;
	}
}

void US1DeathPresentationComponent::StopPresentation()
{
	bIsPresenting = false;
	PresentationElapsed = 0.f;
	SetComponentTickEnabled(false);
	ClearPresentationTimer();
	ResetFadeMaterials();
	FadeMIDs.Empty();
}

void US1DeathPresentationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPresenting == false)
	{
		return;
	}

	PresentationElapsed += DeltaTime;
	const float Alpha = FMath::Clamp(PresentationElapsed / FadeDuration, 0.f, 1.f);
	ApplyFadeAlpha(1.f - Alpha);

	if (PresentationElapsed >= FadeDuration)
	{
		CompletePresentation();
	}
}

void US1DeathPresentationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void US1DeathPresentationComponent::CompletePresentation()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorHiddenInGame(true);
	}

	StopPresentation();
	OnPresentationComplete.Broadcast();
}

void US1DeathPresentationComponent::ClearPresentationTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PresentationTimerHandle);
	}
}

void US1DeathPresentationComponent::InitializeFadeMaterials()
{
	FadeMIDs.Empty();

	if (OpacityParameterName == NAME_None)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComp = Character->GetMesh();
	if (SkeletalMeshComp == nullptr)
	{
		return;
	}

	const int32 MaterialCount = SkeletalMeshComp->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInterface* Material = SkeletalMeshComp->GetMaterial(MaterialIndex);
		if (Material == nullptr)
		{
			continue;
		}

		UMaterialInstanceDynamic* MID = SkeletalMeshComp->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
		if (MID == nullptr)
		{
			continue;
		}

		FadeMIDs.Add(MID);
	}
}

void US1DeathPresentationComponent::ApplyFadeAlpha(float Alpha)
{
	if (OpacityParameterName == NAME_None)
	{
		return;
	}

	const float ClampedAlpha = FMath::Clamp(Alpha, 0.f, 1.f);
	for (UMaterialInstanceDynamic* MID : FadeMIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(OpacityParameterName, ClampedAlpha);
		}
	}
}

void US1DeathPresentationComponent::ResetFadeMaterials()
{
	ApplyFadeAlpha(1.f);
}
