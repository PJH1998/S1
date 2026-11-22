// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "S1Enums.h"
#include "S1MontageProgression_Directional.generated.h"

// 4방향 단발 애니메이션 — 입력 방향에 따라 DirectionalMontages에서 몽타주 선택 후 재생
// SetDirection() → Init() → OnActivated() 순서로 호출
UCLASS()
class S1_API US1MontageProgression_Directional : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual bool WillHandleAbilityEnd() const override { return true; }

	// OnActivated 전에 호출 — 어느 몽타주를 재생할지 결정
	void SetDirection(ES1Direction InDirection) { PendingDirection = InDirection; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<ES1Direction, TObjectPtr<UAnimMontage>> DirectionalMontages;

	ES1Direction PendingDirection = ES1Direction::Forward;

	// 태스크 델리게이트는 무인자다. 어느 몽타주였는지는 태스크가 자기 것만 보므로 불필요하고,
	// 어빌리티 종료 시 태스크가 델리게이트를 직접 끊으므로(StopPlayingMontage) 지연 콜백 가드도 불필요하다
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();
};
