// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1PressurePlate_Gimmick.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AS1Player;
class AS1PressurePlate_Gimmick;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1PlateTriggeredSignature, AS1PressurePlate_Gimmick*, Plate);

/**
 * 플레이어가 밟으면 일정 높이까지 하강해 고정되는 발판.
 * ActivateGimmick() = 하강+고정, ResetGimmick() = 원위치. 상판(Block)과 감지 전용 트리거(Overlap)를 분리해서 쓴다.
 */
UCLASS()
class S1_API AS1PressurePlate_Gimmick : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1PressurePlate_Gimmick();

	UPROPERTY(BlueprintAssignable, Category = "Gimmick|PressurePlate")
	FS1PlateTriggeredSignature OnPlateTriggered;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_IsActivated() override;

	UFUNCTION()
	void OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerVolumeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// 목표 위치(하강/원위치)를 잡고 이동을 시작한다. bInstant면 보간 없이 즉시 스냅(late-join 대응).
	void StartTransition(bool bDescend, bool bInstant);

	// OverlappingPlayers를 순회해 실제로 상판을 딛고 서 있는 플레이어가 있으면 활성화한다.
	// CharacterMovementComponent의 CurrentFloor는 같은 프레임의 이동 스윕(오버랩 발생 시점) 이후에
	// 갱신되므로, 오버랩 콜백에서 바로 검사하지 않고 Tick에서 한 프레임 늦게 검사한다.
	void TryActivateFromOverlap();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlateMesh;

	// PlateMesh에 Attach된 감지 전용 오버랩 볼륨(상판과 별개 — 한 컴포넌트가 Block과 Overlap을 동시에 못 함).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|PressurePlate", meta = (AllowPrivateAccess = "true"))
	float DescendHeight = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|PressurePlate", meta = (AllowPrivateAccess = "true"))
	float DescendDuration = 0.6f;

	FVector InitialLocation = FVector::ZeroVector;
	FVector MoveStartLocation = FVector::ZeroVector;
	FVector MoveTargetLocation = FVector::ZeroVector;
	float MoveElapsedTime = 0.f;
	bool bIsMoving = false;

	// 현재 TriggerVolume과 겹쳐 있는 플레이어들(서버 전용). 바닥 검증은 Tick에서 수행.
	TArray<TWeakObjectPtr<AS1Player>> OverlappingPlayers;
};
