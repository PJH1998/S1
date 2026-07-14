// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1GimmickPuzzle_TogglePillar.generated.h"

class AS1PuzzleButton_Gimmick;
class AS1TogglePillar_Gimmick;
class USceneComponent;
class UBillboardComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1TogglePuzzleCompletedSignature);

/**
 * 버튼(N개)이 각자 인접한 기둥 2개씩을 On/Off로 뒤집어, 모든 기둥이 최고 높이가 되면
 * 클리어되는 라이츠아웃류 퍼즐 코디네이터. AS1GimmickPuzzle_Sequence처럼 PillarOffsets/
 * ButtonOffsets(코디네이터 로컬 좌표)에 따라 기둥·버튼을 직접 스폰해 소유·관리한다
 * (레벨에 별도로 배치하고 참조를 손으로 배선할 필요 없음).
 * 버튼 i는 항상 기둥 i, i+1을 담당하는 체인형 배선으로 고정되며(PillarOffsets.Num()이
 * ButtonOffsets.Num()+1이어야 함), 이 배선은 GF(2)에서 항상 선형독립이라 스크램블이
 * 항상 풀 수 있는 상태로 만들어진다.
 * bIsActivated/IsUsable()은 "퍼즐 완료 여부"로 재해석(Sequence와 동일 관례).
 *
 * 스크램블(어떤 기둥이 최저로 시작할지)은 기둥을 스폰하기 전에 먼저 계산하고, 각 기둥은
 * SpawnActorDeferred + AS1TogglePillar_Gimmick::SetInitialRaised로 BeginPlay 이전에 최종
 * 상태를 확정한 뒤 스폰을 마무리한다 — 기둥이 "일단 기본값으로 떴다가 애니메이션으로 다시
 * 내려가는" 과도 상태 없이, 레벨 시작 시 처음부터 최종 상태로 나타나게 하기 위함이다.
 */
UCLASS()
class S1_API AS1GimmickPuzzle_TogglePillar : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1GimmickPuzzle_TogglePillar();

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FS1TogglePuzzleCompletedSignature OnPuzzleCompleted;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void HandleButtonToggled(AS1PuzzleButton_Gimmick* Button);

	// 기둥의 Tick-Lerp 이동 애니메이션이 끝날 때마다 호출 — 상태는 이미 정답이지만 아직 이동
	// 중이던 기둥이 있어서 완료 처리를 미뤄뒀던 경우, 여기서 다시 확인해 완료 처리한다.
	UFUNCTION()
	void HandlePillarMovementFinished();

private:
	// 서버 전용: 스크램블 결과를 먼저 계산한 뒤 PillarOffsets/ButtonOffsets에 따라 기둥·버튼을
	// 스폰하고(기둥은 SetInitialRaised로 최종 상태를 확정한 뒤 FinishSpawning) 체인형으로 배선한다.
	void SpawnPuzzleActors();

	// 상태(전부 최고 높이)와 이동 애니메이션(전부 정지) 둘 다 만족할 때만 완료 처리한다 —
	// 마지막으로 토글된 기둥이 아직 애니메이션 중이면 HandlePillarMovementFinished가 나중에
	// 다시 호출해줄 때까지 완료를 미룬다.
	void TryCompletePuzzle();

	// 버튼 개수만큼의 비트마스크를 무작위로 골라 "전부 최고 높이" 상태에서 역으로 눌러본 것처럼
	// 최저 상태가 될 기둥 인덱스 집합을 구한다(체인 배선 {i, i+1} 기준의 순수 인덱스 연산이라
	// 실제 스폰 전에도 계산 가능). 결과가 공집합이면(방어적 재추첨) 최소 1개 기둥이 최저 상태가
	// 되도록 보장한다.
	TSet<int32> ComputeScrambleLoweredIndices(int32 ButtonCount) const;

	// 현재 SpawnedPillars가 전부 최고 높이인지 확인.
	bool AreAllPillarsRaised() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	// 코디네이터 로컬 좌표 기준 기둥 배치 위치. 정확히 (ButtonOffsets.Num() + 1)개를 채워야 한다.
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<FVector> PillarOffsets;

	// 코디네이터 로컬 좌표 기준 버튼 배치 위치.
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<FVector> ButtonOffsets;

	// 스폰할 기둥/버튼 클래스(기본값은 베이스 클래스, BP 서브클래스로 비주얼 변형 가능).
	UPROPERTY(EditDefaultsOnly, Category = "Puzzle")
	TSubclassOf<AS1TogglePillar_Gimmick> PillarClass;

	UPROPERTY(EditDefaultsOnly, Category = "Puzzle")
	TSubclassOf<AS1PuzzleButton_Gimmick> ButtonClass;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1TogglePillar_Gimmick>> SpawnedPillars;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1PuzzleButton_Gimmick>> SpawnedButtons;

#if WITH_EDITORONLY_DATA
	// 에디터 전용 배치 미리보기 마커(PillarOffsets/ButtonOffsets 개수·위치를 대략 표시, 게임 중엔 숨김).
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBillboardComponent>> PreviewMarkers;
#endif
};