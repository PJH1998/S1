// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/Loop/S1MontageProgression_Loop.h"
#include "S1Enums.h"
#include "S1MontageProgression_DirectionalLoop.generated.h"

// 방향별 Start-Loop-End 세트 (Strong 등 방향에 따라 다른 Loop 몽타주 3종을 재생해야 하는 경우 사용)
USTRUCT(BlueprintType)
struct FS1DirectionalMontageSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> Start;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> Loop;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> End;
};

// US1MontageProgression_Loop + 방향 선택 — SetDirection()으로 고른 세트의 Start/Loop/End를
// 베이스 클래스의 StartMontage/LoopMontage/EndMontage에 채워넣은 뒤 OnActivated()가 그대로 진행 (Loop 상태머신 로직 재사용)
// SetDirection() → Init() → OnActivated() 순서로 호출 (Directional과 동일 규약)
UCLASS()
class S1_API US1MontageProgression_DirectionalLoop : public US1MontageProgression_Loop
{
	GENERATED_BODY()

public:
	// OnActivated 전에 호출 — 어느 방향 세트를 재생할지 결정
	void SetDirection(ES1Direction InDirection);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<ES1Direction, FS1DirectionalMontageSet> DirectionalMontages;
};
