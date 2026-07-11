// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1SlidingDoor_Gimmick.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/**
 * 퀘스트 클리어 등 외부에서 GimmickTag로 찾아 ActivateGimmick()을 호출하면
 * 좌/우 문짝이 각자의 오프셋 방향으로 슬라이딩해 열리는 자동문.
 * 문 자체엔 트리거 볼륨이 없다 — 활성화는 전적으로 외부 호출에 의존한다.
 */
UCLASS()
class S1_API AS1SlidingDoor_Gimmick : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1SlidingDoor_Gimmick();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_IsActivated() override;

private:
	// 목표 상대위치(열림/닫힘)를 잡고 이동을 시작한다. bInstant면 보간 없이 즉시 스냅(late-join 대응).
	void StartTransition(bool bOpen, bool bInstant);

private:
	// 좌/우 문짝이 대칭 구조라 어느 한쪽을 루트로 삼지 않고 별도 루트를 둔다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DoorRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeftDoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RightDoorMesh;

	// 각 문짝이 닫힘 상대위치 대비 열림까지 이동할 로컬 오프셋.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|SlidingDoor", meta = (AllowPrivateAccess = "true"))
	FVector LeftOpenOffset = FVector(0.f, -150.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|SlidingDoor", meta = (AllowPrivateAccess = "true"))
	FVector RightOpenOffset = FVector(0.f, 150.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|SlidingDoor", meta = (AllowPrivateAccess = "true"))
	float OpenDuration = 1.f;

	FVector LeftClosedRelativeLocation = FVector::ZeroVector;
	FVector RightClosedRelativeLocation = FVector::ZeroVector;
	FVector LeftMoveStartLocation = FVector::ZeroVector;
	FVector LeftMoveTargetLocation = FVector::ZeroVector;
	FVector RightMoveStartLocation = FVector::ZeroVector;
	FVector RightMoveTargetLocation = FVector::ZeroVector;
	float MoveElapsedTime = 0.f;
	bool bIsMoving = false;
};
