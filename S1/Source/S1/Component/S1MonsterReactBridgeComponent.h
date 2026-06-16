// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1Enums.h"
#include "Components/ActorComponent.h"
#include "S1MonsterReactBridgeComponent.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;

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

	static ES1HitReactType ParseHitTypeFromSpec(const FGameplayEffectSpec& Spec);
	static ES1Direction CalcHitDirection(const AActor* Monster, const FVector& HitSourceLocation);
	static bool ShouldPlayHitReact(ES1HitReactType HitType, const UAbilitySystemComponent* ASC);
	static bool TryGetHitSourceLocation(const FGameplayEffectSpec& Spec, FVector& OutLocation);

private:
	FDelegateHandle GameplayEffectAppliedHandle;
};
