// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/NiagaraEffect/S1DamageNumberEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

void AS1DamageNumberEffect::PlayDamageNumber(UWorld* World, int32 Damage, const FVector& HitLocation) const
{
	if (World == nullptr || NiagaraSystem == nullptr)
	{
		return;
	}

	TArray<int32> Digits;
	int32 RemainingDamage = FMath::Abs(Damage);
	if (RemainingDamage == 0)
	{
		Digits.Add(0);
	}
	else
	{
		while (RemainingDamage > 0)
		{
			Digits.Insert(RemainingDamage % 10, 0);
			RemainingDamage /= 10;
		}
	}

	const float RandomAngle = FMath::FRandRange(0.f, 2.f * UE_PI);
	const float RandomDistance = FMath::FRandRange(0.f, RandomRadius);
	const FVector RandomOffset(
		FMath::Cos(RandomAngle) * RandomDistance,
		FMath::Sin(RandomAngle) * RandomDistance,
		0.f);

	FVector CameraRight = FVector::RightVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (const APlayerController* PlayerController = World->GetFirstPlayerController())
	{
		if (const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			SpawnRotation = CameraManager->GetCameraRotation();
			CameraRight = FRotationMatrix(SpawnRotation).GetScaledAxis(EAxis::Y);
		}
	}

	const FVector BaseLocation = HitLocation + RandomOffset;
	const float DigitCenter = (static_cast<float>(Digits.Num()) - 1.f) * 0.5f;

	for (int32 Index = 0; Index < Digits.Num(); ++Index)
	{
		const float DigitOffset = (static_cast<float>(Index) - DigitCenter) * DigitSpacing;
		const FVector DigitLocation = BaseLocation + CameraRight * DigitOffset;

		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			NiagaraSystem,
			DigitLocation,
			SpawnRotation,
			FVector::OneVector,
			true,
			false,
			ENCPoolMethod::AutoRelease);

		if (NiagaraComponent == nullptr)
		{
			continue;
		}

		NiagaraComponent->SetVariableFloat(DigitIndexParameterName, static_cast<float>(Digits[Index]));
		NiagaraComponent->Activate(true);
	}
}
