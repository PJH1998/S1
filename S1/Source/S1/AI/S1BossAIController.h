// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/S1AIController.h"
#include "S1BossAIController.generated.h"

/**
 * 보스 전용 AIController. 일반 몬스터/NPC와 분리해 페이즈 전환용 블랙보드 상태를 관리한다.
 */
UCLASS()
class S1_API AS1BossAIController : public AS1AIController
{
	GENERATED_BODY()

public:
	// 페이즈 인덱스(1/2)를 블랙보드에 기록. 페이즈 전환 GA가 호출.
	void SetBossPhase(int32 PhaseIndex);

	// 페이즈 전환 대기 플래그. HP<50% 감지 시 true, 전환 GA 발동 시 false.
	void SetPhaseTransitionPending(bool bPending);

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI|Boss")
	FName BossPhaseKeyName = TEXT("BossPhase");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Boss")
	FName PhaseTransitionPendingKeyName = TEXT("bPhaseTransitionPending");
};