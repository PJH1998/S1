// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1TogglePillar_Gimmick.generated.h"

class UStaticMeshComponent;
class FLifetimeProperty;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1PillarMovementFinishedSignature);

/**
 * 버튼으로 최고/최저 두 상태만 오가는 기둥(그 외의 중간/기본 상태는 없음).
 * 배치된 트랜스폼(BeginPlay 시점)이 곧 "최고 높이(기본 상태)" 기준이며, 토글되면 거기서
 * LowerHeight만큼 아래로 내려갈 뿐이다 — AS1PressurePlate_Gimmick과 완전히 동일한 방향
 * (배치 지점 = 기본/상단, 활성화 시 그 지점에서 아래로만 이동).
 * bIsActivated == true를 "최저 높이(lowered)"로 재해석해서 쓴다 — 생성자 기본값은 베이스와
 * 동일한 false(= 배치 지점 그대로, 최고 높이). ActivateGimmick()이 아래로, ResetGimmick()이
 * 원래(배치된) 높이로 복귀시킨다. 이동 자체는 AS1SlidingDoor_Gimmick / AS1PressurePlate_Gimmick과
 * 동일한 Tick-Lerp 방식. 코디네이터가 스폰 시 SetInitialRaised()로 최초 상태를 확정하므로,
 * 레벨 시작 시 "일단 기본값으로 떴다가 애니메이션으로 스크램블 상태로 내려가는" 과도 상태는 없다.
 */
UCLASS()
class S1_API AS1TogglePillar_Gimmick : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1TogglePillar_Gimmick();

	// 코디네이터 전용: SpawnActorDeferred로 스폰한 뒤 FinishSpawning 전에 초기 상태를 확정한다.
	// BeginPlay의 순간 스냅이 처음부터 정답 상태를 가리키게 해서, "일단 최고 높이로 뜬 뒤
	// 애니메이션으로 다시 내려가는" 과도 상태(원치 않는 3번째 상태처럼 보임)가 생기지 않게 한다.
	void SetInitialRaised(bool bRaised);

	// 코디네이터 전용: 스폰 전 계산한 "기준(최고 높이) 위치"를 명시적으로 주입한다.
	// bReplicateMovement가 꺼져 있어 액터 트랜스폼 자체의 클라 동기화 시점은 보장되지 않는다
	// (서버가 스폰 직후 자신의 BeginPlay에서 즉시 액터를 이동시킨 뒤에야 클라로 첫 트랜스폼이
	// 넘어가는 경우, 클라가 GetActorLocation()으로 기준을 다시 캡처하면 이미 내려간 위치를
	// 기준으로 오인해 중복으로 더 내려가 버린다) — 그래서 기준 위치는 Replicated 프로퍼티로
	// 별도 동기화하고, BeginPlay에서 GetActorLocation()으로 재캡처하지 않는다.
	void SetSpawnBaseLocation(const FVector& InLocation);

	// 현재 Tick-Lerp 이동 애니메이션이 진행 중인지(코디네이터가 클리어 판정 시 참고).
	bool IsMoving() const { return bIsMoving; }

	// 이동 애니메이션이 끝날 때(Alpha 1.0 도달) 브로드캐스트 — 코디네이터가 "상태는 이미
	// 정답이지만 아직 눈에 보이는 이동이 안 끝난" 기둥의 완료를 기다리는 데 사용.
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|TogglePillar")
	FS1PillarMovementFinishedSignature OnMovementFinished;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_IsActivated() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 목표 위치(최고/최저)를 잡고 이동을 시작한다. bInstant면 보간 없이 즉시 스냅(late-join 대응).
	void StartTransition(bool bLower, bool bInstant);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	// 배치된(최고) 높이에서 얼마나 아래로 내려갈지. 음수를 넣으면 오히려 기준보다 위로
	// 올라가 버리므로(빼기가 더하기가 됨) ClampMin으로 음수 입력을 막는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|TogglePillar", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float LowerHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|TogglePillar", meta = (AllowPrivateAccess = "true"))
	float LowerDuration = 0.6f;

	// 코디네이터가 SetSpawnBaseLocation으로 주입 + 서버→클라 명시적 복제(GetActorLocation()에
	// 의존하지 않음 — 이유는 위 SetSpawnBaseLocation 주석 참고).
	UPROPERTY(Replicated)
	FVector InitialLocation = FVector::ZeroVector;

	FVector MoveStartLocation = FVector::ZeroVector;
	FVector MoveTargetLocation = FVector::ZeroVector;
	float MoveElapsedTime = 0.f;
	bool bIsMoving = false;
};