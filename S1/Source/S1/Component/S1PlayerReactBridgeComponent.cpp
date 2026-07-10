// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1PlayerReactBridgeComponent.h"

#include "AbilitySystem/S1HitReactLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/Player/S1Player.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"

US1PlayerReactBridgeComponent::US1PlayerReactBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void US1PlayerReactBridgeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

void US1PlayerReactBridgeComponent::BindAbilitySystem()
{
	AS1Player* Player = Cast<AS1Player>(GetOwner());
	if (nullptr == Player)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (nullptr == ASC)
	{
		return;
	}

	GameplayEffectAppliedHandle = ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		this, &ThisClass::OnGameplayEffectApplied);
}

void US1PlayerReactBridgeComponent::UnbindAbilitySystem()
{
	AS1Player* Player = Cast<AS1Player>(GetOwner());
	if (nullptr == Player)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (nullptr == ASC || false == GameplayEffectAppliedHandle.IsValid())
	{
		return;
	}

	ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(GameplayEffectAppliedHandle);
	GameplayEffectAppliedHandle.Reset();
}

void US1PlayerReactBridgeComponent::OnGameplayEffectApplied(
	UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle ActiveHandle)
{
	AS1Player* Player = Cast<AS1Player>(GetOwner());
	if (nullptr == Player || false == Player->HasAuthority())
	{
		return;
	}

	const ES1HitReactType HitType = S1HitReactLibrary::ParseHitTypeFromSpec(Spec);
	LOG(TEXT("[ReactBridge] GE applied to %s — parsed HitType: %d"), *Player->GetName(), (int32)HitType);

	FGameplayTag HitTypeTag;
	switch (HitType)
	{
	case ES1HitReactType::Weak:   HitTypeTag = S1HitType::HitType_Weak;   break;
	case ES1HitReactType::Strong: HitTypeTag = S1HitType::HitType_Strong; break;
	case ES1HitReactType::Launch: HitTypeTag = S1HitType::HitType_ToAir;  break;
	default:
		LOG(TEXT("[ReactBridge] HitType None — HitStrengthTag이 안 실려있거나 못 읽음, GA_Hit 트리거 안 함"));
		return;
	}

	AActor* Attacker = Spec.GetEffectContext().GetEffectCauser();
	if (nullptr == Attacker)
	{
		Attacker = Spec.GetEffectContext().GetInstigator();
	}

	FGameplayEventData Payload;
	Payload.EventTag = HitTypeTag;
	Payload.Instigator = Attacker;
	Payload.Target = Player;

	LOG(TEXT("[ReactBridge] HandleGameplayEvent(%s) 발송, Attacker: %s"), *HitTypeTag.ToString(), Attacker ? *Attacker->GetName() : TEXT("None"));
	TargetASC->HandleGameplayEvent(HitTypeTag, &Payload);
}
