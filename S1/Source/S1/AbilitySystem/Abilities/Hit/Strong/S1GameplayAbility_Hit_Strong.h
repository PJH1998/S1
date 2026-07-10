// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "S1GameplayAbility_Hit_Strong.generated.h"

// 강한 피격 — 공격원 반대 방향으로 넉백(ToAir보다 낮은 포물선) + 앞/뒤 피격에 따라 회전 분기
// (앞: 공격원을 정면으로 바라봄 / 뒤: 공격원을 등지고(뒤로 두고) 바라봄 — 두 경우 다 넉백은 공격원 반대 방향으로 동일)
// MontageProgression은 항상 US1MontageProgression_DirectionalLoop 고정(생성자에서 직접 생성) — DirectionalMontages는 Forward/Back 두 키만 사용
UCLASS()
class S1_API US1GameplayAbility_Hit_Strong : public US1GameplayAbility_Hit
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Hit_Strong(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 넉백 수평 속도(cm/s) — 공격 쪽에서 전달받지 않는 고정값
	UPROPERTY(EditDefaultsOnly, Category = "Hit|Strong")
	float LaunchSpeed = 600.f;

	// 넉백에 더해지는 낮은 상승 속도(cm/s) — ToAir보다 훨씬 낮게 잡아서 살짝 뜨는 정도의 포물선
	UPROPERTY(EditDefaultsOnly, Category = "Hit|Strong")
	float LaunchUpSpeed = 150.f;
};
