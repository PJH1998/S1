// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1GimmickPuzzle_Sequence.generated.h"

class AS1PressurePlate_Gimmick;
class UBillboardComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1PuzzleSequenceCompletedSignature);

/**
 * 여러 발판(AS1PressurePlate_Gimmick)을 정해진 순서로 밟아야 클리어되는 순서 퍼즐 코디네이터.
 * 자신을 기준으로 한 상대 오프셋(PlateOffsets)에 따라 발판을 직접 스폰·배치한다(레벨에 여러 곳
 * 재배치하기 쉬움). AS1GimmickObject를 상속해 US1GimmickManager에 그대로 등록/조회되며,
 * bIsActivated/IsUsable()은 "퍼즐 완료 여부"로, ActivateGimmick()은 "완료 처리"로,
 * ResetGimmick()은 "진행 상태 초기화(발판 전부 원위치 + 인덱스 0)"로 재해석해서 쓴다.
 */
UCLASS()
class S1_API AS1GimmickPuzzle_Sequence : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1GimmickPuzzle_Sequence();

	// 서버 전용: 진행 상태 초기화(오답 시 내부 호출, 또는 GimmickManager->ResetGimmicks(Tag)로 외부 호출).
	virtual void ResetGimmick() override;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle")
	FS1PuzzleSequenceCompletedSignature OnPuzzleCompleted;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// 완료 전환 시(서버 호출·클라 OnRep 공통) 먼지 이펙트 연출. 리셋 전환 시엔 아무것도 하지 않음.
	virtual void OnRep_IsActivated() override;

	UFUNCTION()
	void HandlePlateTriggered(AS1PressurePlate_Gimmick* Plate);

private:
	// 서버 전용: PlateOffsets에 따라 발판을 스폰하고 SpawnedPlates에 순서대로 채운다.
	void SpawnPlates();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	// 코디네이터 로컬 좌표 기준 발판 상대 위치. 배열 순서 = 정답 순서.
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<FVector> PlateOffsets;

	// 스폰할 발판 클래스(기본값은 AS1PressurePlate_Gimmick, BP 서브클래스로 비주얼 변형 가능).
	UPROPERTY(EditDefaultsOnly, Category = "Puzzle")
	TSubclassOf<AS1PressurePlate_Gimmick> PlateClass;

	// 퍼즐 완료 보상 조회용 태그(US1AssetManager 태그→에셋 매핑으로 드랍테이블 조회). 실제 지급 호출은
	// 이 코디네이터가 아니라 레벨/BP에서 OnPuzzleCompleted를 바인딩해 US1DropManager를 통해 수행한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle", meta = (AllowPrivateAccess = "true"))
	FGameplayTag RewardDropTableTag;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1PressurePlate_Gimmick>> SpawnedPlates;

	int32 CurrentStepIndex = 0;

#if WITH_EDITORONLY_DATA
	// 에디터 전용 배치 미리보기 마커(PlateOffsets 개수/위치를 대략적으로 표시, 게임 중엔 숨김).
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBillboardComponent>> PreviewMarkers;
#endif
};
