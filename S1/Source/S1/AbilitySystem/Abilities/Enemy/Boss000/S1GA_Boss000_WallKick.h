// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/S1GA_Enemy.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "S1GA_Boss000_WallKick.generated.h"

class AS1Boss000_Gimmick;
class AS1Boss_000;
class UAnimMontage;
class US1GimmickManager;

/**
 * Boss000 Wall-Kick 패턴.
 * 단일 활성화로 3개 몽타주를 엮는다: Start(바닥→기둥) → Move×N(기둥→다른 기둥) → SlashThrough(→플레이어).
 * 각 페이즈 몽타주 완료 시점에 다음 페이즈를 재생한다. 기둥은 US1GimmickManager에 태그로 질의(직접 참조 없음).
 * 실제 이동은 각 AM에 배치된 S1ANS_WallKickDash가 보스에 설정된 타깃(소켓/플레이어)으로 직선 수행한다.
 */
UCLASS()
class S1_API US1GA_Boss000_WallKick : public US1GA_Enemy
{
	GENERATED_BODY()

public:
	US1GA_Boss000_WallKick(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnPhaseCompleted();

	UFUNCTION()
	void OnPhaseInterrupted();

	// Slash 루프 거리 판정(몽타주 내 CheckDistance 이벤트마다). 가까우면 Loop→End로 전환.
	UFUNCTION()
	void OnSlashCheckDistance(FGameplayEventData Payload);

private:
	enum class EWallKickPhase : uint8 { Start, Move, Slash };

	void PlayPhaseMontage(const FGameplayTag& PhaseMontageTag);
	UAnimMontage* FindPhaseMontage(const FGameplayTag& PhaseMontageTag) const;

	// 다음 기둥(현재 기둥 제외)으로 Move 페이즈 시작. 사용 가능한 기둥이 없으면 false.
	bool StartMovePhase();
	// 플레이어를 향해 SlashThrough 페이즈 시작.
	void StartSlashPhase();

	AS1Boss_000* GetMonster() const;
	US1GimmickManager* GetGimmickManager() const;

	// 현재 타깃(플레이어) 위치. 없으면 보스 전방 폴백.
	FVector ResolveTargetLocation() const;

	// 타깃 방향으로 보스를 즉시 회전(페이즈 시작 시 호출 — 이동 전 어색한 방향 응시 방지).
	void FaceTowards(const FVector& TargetLocation) const;

	// 패턴 동안 중력 off + 캡슐 콜리전 off(기둥에 안 걸리고 올라탄 상태 유지), 종료 시 복원. 어빌리티 단위.
	void BeginPatternMovement();
	void EndPatternMovement();

private:
	UPROPERTY(EditDefaultsOnly, Category = "WallKick")
	FGameplayTag GimmickTag;

	UPROPERTY(EditDefaultsOnly, Category = "WallKick")
	FGameplayTag StartMontageTag;

	UPROPERTY(EditDefaultsOnly, Category = "WallKick")
	FGameplayTag MoveMontageTag;

	UPROPERTY(EditDefaultsOnly, Category = "WallKick")
	FGameplayTag SlashMontageTag;

	UPROPERTY(EditDefaultsOnly, Category = "WallKick", meta = (ClampMin = "1"))
	int32 MinHops = 2;

	UPROPERTY(EditDefaultsOnly, Category = "WallKick", meta = (ClampMin = "1"))
	int32 MaxHops = 3;

	// SlashThrough 섹션 구성(Attack01과 동일한 거리 루프).
	UPROPERTY(EditDefaultsOnly, Category = "WallKick|Slash")
	FName SlashStartSection = FName("Start");

	UPROPERTY(EditDefaultsOnly, Category = "WallKick|Slash")
	FName SlashLoopSection = FName("Loop");

	UPROPERTY(EditDefaultsOnly, Category = "WallKick|Slash")
	FName SlashEndSection = FName("End");

	// SlashThrough Loop 중 거리 판정을 트리거하는 이벤트(몽타주 Notify가 발사).
	UPROPERTY(EditDefaultsOnly, Category = "WallKick|Slash")
	FGameplayTag CheckDistanceEventTag;

	// 이 거리(2D) 이내가 되면 Loop를 벗어나 End(타격)로 전환.
	UPROPERTY(EditDefaultsOnly, Category = "WallKick|Slash")
	float SlashApproachDistance = 300.f;

	EWallKickPhase CurrentPhase = EWallKickPhase::Start;
	int32 HopCount = 0;
	int32 TargetHops = 0;
	TWeakObjectPtr<AS1Boss000_Gimmick> CurrentPillar;

	// 패턴 이동 상태 복원용(중력 + 캡슐 콜리전).
	float CachedGravityScale = 1.f;
	bool bPatternMovementActive = false;

	// 페이즈 전환 중(이전 몽타주 블렌드아웃 중에 다음 것을 시작) 자기-중단 콜백 무시용.
	bool bAdvancing = false;
};
