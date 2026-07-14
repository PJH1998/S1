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

FGameplayTag S1HitReactLibrary::HitReactTypeToTag(ES1HitReactType HitType)
{
	switch (HitType)
	{
	case ES1HitReactType::Weak:   return S1HitType::HitType_Weak;
	case ES1HitReactType::Strong: return S1HitType::HitType_Strong;
	case ES1HitReactType::Launch: return S1HitType::HitType_ToAir;
	default: break;
	}

	return FGameplayTag::EmptyTag;
}

FGameplayTag S1HitReactLibrary::FindSoundBaseTag(const FGameplayEffectSpec& Spec)
{
	static const FGameplayTag SoundHitRoot = FGameplayTag::RequestGameplayTag(FName("Sound.Hit"), false);
	if (false == SoundHitRoot.IsValid())
	{
		return FGameplayTag::EmptyTag;
	}

	FGameplayTagContainer AllTags;
	AllTags.AppendTags(Spec.DynamicGrantedTags);
	AllTags.AppendTags(Spec.GetDynamicAssetTags());

	for (const FGameplayTag& Tag : AllTags)
	{
		if (Tag.MatchesTag(SoundHitRoot))
		{
			return Tag;
		}
	}

	return FGameplayTag::EmptyTag;
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
