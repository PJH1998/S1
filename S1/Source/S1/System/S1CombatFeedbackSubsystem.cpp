// Fill out your copyright notice in the Description page of Project Settings.


#include "System/S1CombatFeedbackSubsystem.h"

#include "Camera/PlayerCameraManager.h"
#include "Data/S1EffectData.h"
#include "Effect/NiagaraEffect/S1DamageNumberEffect.h"
#include "Effect/S1Effect.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

void US1CombatFeedbackSubsystem::Deinitialize()
{
	for (FS1ActiveDamageNumberGroup& Group : ActiveDamageNumberGroups)
	{
		FinishDamageNumberGroup(Group);
	}
	ActiveDamageNumberGroups.Reset();
	Super::Deinitialize();
}

void US1CombatFeedbackSubsystem::Tick(float DeltaTime)
{
	for (int32 GroupIndex = ActiveDamageNumberGroups.Num() - 1; GroupIndex >= 0; --GroupIndex)
	{
		FS1ActiveDamageNumberGroup& Group = ActiveDamageNumberGroups[GroupIndex];
		Group.ElapsedTime += DeltaTime;

		if (Group.ElapsedTime >= Group.Lifetime)
		{
			FinishDamageNumberGroup(Group);
			ActiveDamageNumberGroups.RemoveAtSwap(GroupIndex);
			continue;
		}

		UpdateDamageNumberGroup(Group);
	}
}

TStatId US1CombatFeedbackSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(US1CombatFeedbackSubsystem, STATGROUP_Tickables);
}

bool US1CombatFeedbackSubsystem::IsTickable() const
{
	return ActiveDamageNumberGroups.Num() > 0;
}

void US1CombatFeedbackSubsystem::ShowDamageNumber(int32 Damage, const FVector& HitLocation)
{
	if (Damage < 0)
	{
		Damage = FMath::Abs(Damage);
	}

	const AS1DamageNumberEffect* DamageNumberEffect = GetDamageNumberEffect();
	if (DamageNumberEffect == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TArray<int32> Digits;
	SplitDamageIntoDigits(Damage, Digits);
	if (Digits.IsEmpty())
	{
		return;
	}

	const float RandomAngle = FMath::FRandRange(0.f, 2.f * UE_PI);
	const float RandomDistance = FMath::FRandRange(0.f, DamageNumberEffect->GetRandomRadius());
	const FVector RandomOffset(
		FMath::Cos(RandomAngle) * RandomDistance,
		FMath::Sin(RandomAngle) * RandomDistance,
		0.f);

	FS1ActiveDamageNumberGroup Group;
	Group.CenterWorldLocation = HitLocation + RandomOffset;
	Group.Lifetime = DamageNumberEffect->GetLifetime();
	Group.RiseSpeed = DamageNumberEffect->GetRiseSpeed();
	Group.DigitSpacing = DamageNumberEffect->GetDigitSpacing();

	const float DigitCenter = (static_cast<float>(Digits.Num()) - 1.f) * 0.5f;
	Group.Digits.Reserve(Digits.Num());

	for (int32 Index = 0; Index < Digits.Num(); ++Index)
	{
		UNiagaraComponent* DigitComponent = DamageNumberEffect->SpawnDigitComponent(World, Digits[Index]);
		if (DigitComponent == nullptr)
		{
			continue;
		}

		FS1DamageDigitInstance DigitInstance;
		DigitInstance.Component = DigitComponent;
		DigitInstance.OffsetFromCenter = static_cast<float>(Index) - DigitCenter;
		Group.Digits.Add(DigitInstance);
	}

	if (Group.Digits.IsEmpty())
	{
		return;
	}

	UpdateDamageNumberGroup(Group);
	ActiveDamageNumberGroups.Add(MoveTemp(Group));
}

void US1CombatFeedbackSubsystem::SplitDamageIntoDigits(int32 Damage, TArray<int32>& OutDigits)
{
	OutDigits.Reset();

	int32 RemainingDamage = FMath::Abs(Damage);
	if (RemainingDamage == 0)
	{
		OutDigits.Add(0);
		return;
	}

	while (RemainingDamage > 0)
	{
		OutDigits.Insert(RemainingDamage % 10, 0);
		RemainingDamage /= 10;
	}
}

bool US1CombatFeedbackSubsystem::GetCameraBasis(UWorld* World, FVector& OutRight, FRotator& OutRotation)
{
	if (World == nullptr)
	{
		return false;
	}

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		return false;
	}

	const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (CameraManager == nullptr)
	{
		return false;
	}

	OutRotation = CameraManager->GetCameraRotation();
	OutRight = FRotationMatrix(OutRotation).GetScaledAxis(EAxis::Y);
	return true;
}

void US1CombatFeedbackSubsystem::UpdateDamageNumberGroup(FS1ActiveDamageNumberGroup& Group) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FVector CameraRight = FVector::RightVector;
	FRotator CameraRotation = FRotator::ZeroRotator;
	GetCameraBasis(World, CameraRight, CameraRotation);

	const FVector GroupLocation = Group.CenterWorldLocation + FVector::UpVector * (Group.RiseSpeed * Group.ElapsedTime);

	for (const FS1DamageDigitInstance& Digit : Group.Digits)
	{
		if (Digit.Component == nullptr)
		{
			continue;
		}

		const FVector DigitLocation = GroupLocation + CameraRight * (Digit.OffsetFromCenter * Group.DigitSpacing);
		Digit.Component->SetWorldLocationAndRotation(DigitLocation, CameraRotation);
	}
}

void US1CombatFeedbackSubsystem::FinishDamageNumberGroup(FS1ActiveDamageNumberGroup& Group)
{
	for (const FS1DamageDigitInstance& Digit : Group.Digits)
	{
		if (Digit.Component == nullptr)
		{
			continue;
		}

		Digit.Component->Deactivate();
		Digit.Component->DestroyComponent();
	}

	Group.Digits.Reset();
}

const AS1DamageNumberEffect* US1CombatFeedbackSubsystem::GetDamageNumberEffect() const
{
	US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(S1AssetTags::Asset_Effect);
	if (EffectData == nullptr)
	{
		return nullptr;
	}

	const TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(S1EffectTags::Effect_DamageNumber);
	if (EffectClass == nullptr)
	{
		return nullptr;
	}

	const AS1DamageNumberEffect* DamageNumberEffect = Cast<AS1DamageNumberEffect>(EffectClass->GetDefaultObject());
	if (DamageNumberEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Effect.DamageNumber must use AS1DamageNumberEffect."));
	}

	return DamageNumberEffect;
}
