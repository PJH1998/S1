// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/S1BossSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/S1HitReactLibrary.h"
#include "Character/Boss/S1BossBase.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"

void US1BossSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 가드 정면 차단: Health 데미지 + 가드 상태 + 정면 공격이면 데미지를 무효화한다.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && Data.EvaluatedData.Magnitude < 0.f)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		AS1BossBase* Boss = ASC ? Cast<AS1BossBase>(ASC->GetAvatarActor()) : nullptr;
		if (Boss && ASC->HasMatchingGameplayTag(S1StateTags::State_Boss_Guard))
		{
			AActor* Attacker = Data.EffectSpec.GetContext().GetEffectCauser();
			if (nullptr == Attacker)
			{
				Attacker = Data.EffectSpec.GetContext().GetInstigator();
			}

			if (Attacker)
			{
				FVector ToAttacker = Attacker->GetActorLocation() - Boss->GetActorLocation();
				ToAttacker.Z = 0.f;
				if (false == ToAttacker.IsNearlyZero())
				{
					const float ForwardDot = FVector::DotProduct(Boss->GetActorForwardVector(), ToAttacker.GetSafeNormal());
					const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Boss->GetGuardFrontalHalfAngleDeg()));
					if (ForwardDot >= CosHalfAngle)
					{
						// 정면 콘 안 → 차단. 적용된 데미지 원복 후 Super(일반 히트음/데미지 넘버/사망) 스킵.
						SetHealth(GetHealth() - Data.EvaluatedData.Magnitude);

						// 가드음: 일반 히트음 시스템을 HitType::Guard로 재사용 → Sound.Hit.<무기>.Guard(무기별). 블록 넘버는 별도 유지.
						const FGameplayTag SoundBaseTag = S1HitReactLibrary::FindSoundBaseTag(Data.EffectSpec);
						Boss->MulticastPlayHitSound(SoundBaseTag, ES1HitReactType::Guard, Boss->GetActorLocation());
						Boss->MulticastNotifyGuardBlocked(Boss->GetActorLocation());
						return;
					}
				}
			}
		}
	}

	Super::PostGameplayEffectExecute(Data);

	// HP 50% 미만 페이즈 전환 감지(차단되지 않은 정상 데미지 경로). GetHealth()는 Super의 Clamp 이후 값.
	if (Data.EvaluatedData.Attribute != GetHealthAttribute() || Data.EvaluatedData.Magnitude >= 0.f)
	{
		return;
	}
	if (GetHealth() <= 0.f)
	{
		return;
	}
	if (GetMaxHealth() > 0.f && GetHealth() / GetMaxHealth() < 0.5f)
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			if (AS1BossBase* Boss = Cast<AS1BossBase>(ASC->GetAvatarActor()))
			{
				Boss->RequestPhaseTransition();
			}
		}
	}

	// HP 20% 미만 저HP BGM 전환 감지.
	if (GetMaxHealth() > 0.f && GetHealth() / GetMaxHealth() < 0.2f)
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			if (AS1BossBase* Boss = Cast<AS1BossBase>(ASC->GetAvatarActor()))
			{
				Boss->RequestLowHpBgm();
			}
		}
	}
}

