// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1MonsterReactBridgeComponent.h"

#include "AbilitySystemComponent.h"
#include "AI/S1AIController.h"
#include "Character/S1Monster.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Tags/S1GameplayTags.h"

US1MonsterReactBridgeComponent::US1MonsterReactBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void US1MonsterReactBridgeComponent::BeginPlay()
{
	Super::BeginPlay();
	BindAbilitySystem();
}

void US1MonsterReactBridgeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

void US1MonsterReactBridgeComponent::BindAbilitySystem()
{
	AS1Monster* Monster = Cast<AS1Monster>(GetOwner());
	if (nullptr == Monster)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Monster->GetAbilitySystemComponent();
	if (nullptr == ASC)
	{
		return;
	}

	GameplayEffectAppliedHandle = ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		this, &ThisClass::OnGameplayEffectApplied);
}

void US1MonsterReactBridgeComponent::UnbindAbilitySystem()
{
	AS1Monster* Monster = Cast<AS1Monster>(GetOwner());
	if (nullptr == Monster)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Monster->GetAbilitySystemComponent();
	if (nullptr == ASC || false == GameplayEffectAppliedHandle.IsValid())
	{
		return;
	}

	ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(GameplayEffectAppliedHandle);
	GameplayEffectAppliedHandle.Reset();
}

void US1MonsterReactBridgeComponent::OnGameplayEffectApplied(
	UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle ActiveHandle)
{
	AS1Monster* Monster = Cast<AS1Monster>(GetOwner());
	if (nullptr == Monster || Monster->IsDead())
	{
		return;
	}

	if (false == Monster->HasAuthority())
	{
		return;
	}

	const ES1HitReactType HitType = ParseHitTypeFromSpec(Spec);
	if (HitType == ES1HitReactType::None)
	{
		return;
	}

	if (false == ShouldPlayHitReact(HitType, TargetASC))
	{
		return;
	}

	FVector HitSourceLocation = FVector::ZeroVector;
	if (false == TryGetHitSourceLocation(Spec, HitSourceLocation))
	{
		return;
	}

	const ES1Direction HitDirection = CalcHitDirection(Monster, HitSourceLocation);

	AActor* Attacker = Spec.GetEffectContext().GetEffectCauser();
	if (nullptr == Attacker)
	{
		Attacker = Spec.GetEffectContext().GetInstigator();
	}

	if (AS1AIController* AIController = Cast<AS1AIController>(Monster->GetController()))
	{
		AIController->SetHitReactRequest(HitType, HitDirection, Attacker);
	}
}

ES1HitReactType US1MonsterReactBridgeComponent::ParseHitTypeFromSpec(const FGameplayEffectSpec& Spec)
{
	FGameplayTagContainer AllTags;
	AllTags.AppendTags(Spec.DynamicGrantedTags);
	const FGameplayTagContainer& DynamicAssetTags = Spec.GetDynamicAssetTags();
	AllTags.AppendTags(DynamicAssetTags);
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

ES1Direction US1MonsterReactBridgeComponent::CalcHitDirection(const AActor* Monster, const FVector& HitSourceLocation)
{
	if (nullptr == Monster)
	{
		return ES1Direction::Forward;
	}

	FVector ToSource = HitSourceLocation - Monster->GetActorLocation();
	ToSource.Z = 0.f;
	if (ToSource.IsNearlyZero())
	{
		return ES1Direction::Forward;
	}

	const FVector Dir = ToSource.GetSafeNormal();
	const float ForwardDot = FVector::DotProduct(Monster->GetActorForwardVector(), Dir);
	const float RightDot = FVector::DotProduct(Monster->GetActorRightVector(), Dir);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? ES1Direction::Forward : ES1Direction::Back;
	}

	return RightDot >= 0.f ? ES1Direction::Right : ES1Direction::Left;
}

bool US1MonsterReactBridgeComponent::ShouldPlayHitReact(ES1HitReactType HitType, const UAbilitySystemComponent* ASC)
{
	if (nullptr == ASC)
	{
		return false;
	}

	if (false == ASC->HasMatchingGameplayTag(S1StateTags::State_Enemy_Attacking))
	{
		return true;
	}

	return HitType == ES1HitReactType::Strong || HitType == ES1HitReactType::Launch;
}

bool US1MonsterReactBridgeComponent::TryGetHitSourceLocation(const FGameplayEffectSpec& Spec, FVector& OutLocation)
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
