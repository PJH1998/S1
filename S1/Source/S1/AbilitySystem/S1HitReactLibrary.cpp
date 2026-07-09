// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/S1HitReactLibrary.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Tags/S1GameplayTags.h"

ES1HitReactType S1HitReactLibrary::ParseHitTypeFromSpec(const FGameplayEffectSpec& Spec)
{
	FGameplayTagContainer AllTags;
	AllTags.AppendTags(Spec.DynamicGrantedTags);
	AllTags.AppendTags(Spec.GetDynamicAssetTags());
	if (nullptr != Spec.Def)
	{
		AllTags.AppendTags(Spec.Def->GetGrantedTags());
	}

	if (AllTags.HasTag(S1HitType::HitType_ToAir))
	{
		return ES1HitReactType::Launch;
	}
	if (AllTags.HasTag(S1HitType::HitType_Strong))
	{
		return ES1HitReactType::Strong;
	}
	if (AllTags.HasTag(S1HitType::HitType_Weak))
	{
		return ES1HitReactType::Weak;
	}

	return ES1HitReactType::None;
}

ES1Direction S1HitReactLibrary::CalcHitDirection(const AActor* Target, const FVector& HitSourceLocation)
{
	if (nullptr == Target)
	{
		return ES1Direction::Forward;
	}

	FVector ToSource = HitSourceLocation - Target->GetActorLocation();
	ToSource.Z = 0.f;
	if (ToSource.IsNearlyZero())
	{
		return ES1Direction::Forward;
	}

	const FVector Dir = ToSource.GetSafeNormal();
	const float ForwardDot = FVector::DotProduct(Target->GetActorForwardVector(), Dir);
	const float RightDot = FVector::DotProduct(Target->GetActorRightVector(), Dir);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? ES1Direction::Forward : ES1Direction::Back;
	}

	return RightDot >= 0.f ? ES1Direction::Right : ES1Direction::Left;
}

bool S1HitReactLibrary::TryGetHitSourceLocation(const FGameplayEffectSpec& Spec, FVector& OutLocation)
{
	if (AActor* Causer = Spec.GetEffectContext().GetEffectCauser())
	{
		OutLocation = Causer->GetActorLocation();
		return true;
	}

	if (AActor* Instigator = Spec.GetEffectContext().GetInstigator())
	{
		OutLocation = Instigator->GetActorLocation();
		return true;
	}

	return false;
}
