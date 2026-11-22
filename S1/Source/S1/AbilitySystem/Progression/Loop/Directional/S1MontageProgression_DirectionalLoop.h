// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "S1Enums.h"
#include "S1MontageProgression_DirectionalLoop.generated.h"

// US1MontageProgression_Loop + 방향 선택 — SetDirection()으로 고른 몽타주를 베이스의 SectionMontage에
// 채워넣은 뒤 OnActivated()가 그대로 진행한다 (Loop 상태머신 로직 재사용).
// 섹션 이름(Start/Loop/End)은 베이스 프로퍼티를 공용으로 쓰므로 방향마다 따로 적지 않는다.
// SetDirection() → Init() → OnActivated() 순서로 호출 (Directional과 동일 규약)
UCLASS()
class S1_API US1MontageProgression_DirectionalLoop : public US1MontageProgression_Loop
{
	GENERATED_BODY()

public:
	// OnActivated 전에 호출 — 어느 방향 몽타주를 재생할지 결정
	void SetDirection(ES1Direction InDirection);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<ES1Direction, TObjectPtr<UAnimMontage>> DirectionalMontages;
};