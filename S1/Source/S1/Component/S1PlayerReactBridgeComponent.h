// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1PlayerReactBridgeComponent.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;

// 플레이어 ASC에 적용된 GE에서 HitType을 감지해 GA_Hit 계열을 트리거 — S1MonsterReactBridgeComponent의 GAS 버전
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1PlayerReactBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1PlayerReactBridgeComponent();

	// AS1Player::PossessedBy()에서 InitSystem() 이후(AbilitySystemComponent가 실제로 세팅된 시점)에 호출
	// BeginPlay 시점엔 ASC가 아직 PlayerState에서 안 넘어와서(null) 여기서 바인딩하면 안 됨
	void BindAbilitySystem();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UnbindAbilitySystem();

	void OnGameplayEffectApplied(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveHandle);

private:
	FDelegateHandle GameplayEffectAppliedHandle;
};
