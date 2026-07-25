// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/S1MonsterReactBridgeComponent.h"

#include "AbilitySystem/S1HitReactLibrary.h"
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

	const ES1HitReactType HitType = S1HitReactLibrary::ParseHitTypeFromSpec(Spec);
	if (HitType == ES1HitReactType::None)
	{
		return;
	}

	// 사운드는 US1MonsterSet::PostGameplayEffectExecute가 MulticastPlayHitSound로 처리 — 여기선 AI 리액션만(서버 전용)
	if (false == Monster->HasAuthority())
	{
		return;
	}

	if (false == ShouldPlayHitReact(HitType, TargetASC))
	{
		return;
	}

	FVector HitSourceLocation = FVector::ZeroVector;
	if (false == S1HitReactLibrary::TryGetHitSourceLocation(Spec, HitSourceLocation))
	{
		return;
	}

	const ES1Direction HitDirection = S1HitReactLibrary::CalcHitDirection(Monster, HitSourceLocation);

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
