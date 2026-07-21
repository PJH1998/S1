// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1Enums.h"
#include "Components/ActorComponent.h"
#include "S1MonsterReactBridgeComponent.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
class AS1Monster;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1MonsterReactBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1MonsterReactBridgeComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void BindAbilitySystem();
	void UnbindAbilitySystem();

	void OnGameplayEffectApplied(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveHandle);

	// 권위 여부와 무관하게(각 머신 로컬) 재생 — HasAuthority 체크보다 먼저 호출해야 함
	void PlayHitSound(AS1Monster* Monster, ES1HitReactType HitType, const FGameplayEffectSpec& Spec);

	static bool ShouldPlayHitReact(ES1HitReactType HitType, const UAbilitySystemComponent* ASC);

private:
	FDelegateHandle GameplayEffectAppliedHandle;
};
