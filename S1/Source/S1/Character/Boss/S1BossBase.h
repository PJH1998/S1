// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Monster.h"
#include "S1BossBase.generated.h"

class US1BossSet;

/**
 * 
 */
UCLASS()
class S1_API AS1BossBase : public AS1Monster
{
	GENERATED_BODY()
	
public:
	AS1BossBase();

public:
	virtual ES1EnemyTier GetLockOnTier_Implementation() override;

public:
	US1BossSet* GetS1BossSet() const;
	const FName& GetBossName() const;

	// HP 50% 미만 감지 시 호출(서버). 1회만 블랙보드 페이즈 전환 펜딩 플래그를 세운다.
	void RequestPhaseTransition();

	// 가드 정면 차단 콘의 절반 각도(도). S1BossSet의 방향 판정이 읽는다.
	float GetGuardFrontalHalfAngleDeg() const { return GuardFrontalHalfAngleDeg; }

	// 가드 차단 발생을 전체 클라(+서버)에 알린다. S1BossSet의 정면 차단 분기에서 호출된다.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastNotifyGuardBlocked(const FVector& InBlockLocation);

protected:
	FName BossName = {};

	// 가드 중 이 각도 안(보스 정면 기준)에서 들어오는 공격은 무시한다.
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Guard")
	float GuardFrontalHalfAngleDeg = 60.f;

private:
	// 페이즈 전환 1회성 보장(서버 전용 transient).
	bool bPhaseTransitionTriggered = false;
};
