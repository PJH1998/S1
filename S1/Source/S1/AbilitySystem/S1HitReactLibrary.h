// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1Enums.h"

struct FGameplayEffectSpec;
class AActor;

// 몬스터(BT)/플레이어(GAS) 양쪽의 피격 리액션 컴포넌트가 공유하는 판정 로직
namespace S1HitReactLibrary
{
	// GE Spec의 태그(DynamicGrantedTags/DynamicAssetTags/GrantedTags)에서 HitType 파싱 — 없으면 None
	ES1HitReactType ParseHitTypeFromSpec(const FGameplayEffectSpec& Spec);

	// 피격자 기준 공격원 위치로 4방향(Forward/Back/Left/Right) 판정
	ES1Direction CalcHitDirection(const AActor* Target, const FVector& HitSourceLocation);

	// GE Context에서 공격원(EffectCauser 우선, 없으면 Instigator) 위치를 얻음 — 실패 시 false
	bool TryGetHitSourceLocation(const FGameplayEffectSpec& Spec, FVector& OutLocation);
}
