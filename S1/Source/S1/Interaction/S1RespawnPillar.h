// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interface/S1InteractableInterface.h"
#include "S1RespawnPillar.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class AS1PlayerState;

/**
 * 상호작용 시 상호작용한 플레이어의 AS1PlayerState::LastSpawnPointTag를 이 기둥의 SpawnPointTag로 설정한다.
 * "활성화" 여부는 리플리케이트하지 않는다 — 플레이어마다 마지막 스폰 지점이 다를 수 있어서, 각 클라이언트가
 * 자신의 로컬 플레이어 PlayerState의 LastSpawnPointTag와 SpawnPointTag를 비교해 로컬로만 색을 갱신한다.
 */
UCLASS()
class S1_API AS1RespawnPillar : public AActor, public IS1InteractableInterface
{
	GENERATED_BODY()

public:
	AS1RespawnPillar();

	// IS1InteractableInterface: 서버 전용 — 상호작용한 Pawn의 PlayerState에 SpawnPointTag를 설정하고 HP를 Full 회복한다.
	virtual void Interact(AActor* Interactor) override;
	// 로컬 기준 이미 활성화된 기둥이면 false — 후보에서 제외되어 프롬프트가 안 뜨고 재상호작용도 막힌다(중복 활성화 방지).
	virtual bool CanInteract() const override { return false == bIsActivatedLocally; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 로컬 플레이어 PlayerState를 찾아 변경 알림을 구독한다. Possess/PlayerState 셋업 타이밍 레이스 대비 재시도.
	void BindLocalPlayerState();
	void OnLocalSpawnPointTagChanged(FGameplayTag NewTag);
	void RefreshActivatedVisual();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RespawnPillar", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RespawnPillar", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionCollision;

	// DataTable.SpawnPoint 조회 태그와 동일해야 함(S1SpawnTags::Spawn_* 재사용).
	UPROPERTY(EditAnywhere, Category = "RespawnPillar")
	FGameplayTag SpawnPointTag;

	UPROPERTY(EditAnywhere, Category = "RespawnPillar|Sound")
	FGameplayTag InteractionSoundTag;

	// 머티리얼의 활성화 Scalar 파라미터 이름 — BP에서 머티리얼에 맞게 재지정 가능.
	UPROPERTY(EditDefaultsOnly, Category = "RespawnPillar|Visual")
	FName ActivatedParamName = TEXT("Activate");

	// 초당 변화량(0~1 기준) — 활성/비활성 전환 시 이 파라미터가 즉시 스냅되지 않고 서서히 바뀌도록.
	UPROPERTY(EditDefaultsOnly, Category = "RespawnPillar|Visual")
	float FadeSpeed = 2.f;

	// 활성화 상태일 때 메시가 위아래로 왕복하는 폭(cm)/속도.
	UPROPERTY(EditDefaultsOnly, Category = "RespawnPillar|Visual")
	float OscillateAmplitude = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "RespawnPillar|Visual")
	float OscillateSpeed = 2.f;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	TWeakObjectPtr<AS1PlayerState> BoundPlayerState;

	// RefreshActivatedVisual()에서 갱신 — CanInteract()가 매번 다시 계산하지 않도록 캐시.
	bool bIsActivatedLocally = false;

	// Tick에서 ActivatedParamName을 향해 서서히 보간되는 실제 값(0~1).
	float CurrentActivatedAlpha = 0.f;

	float OscillateElapsedTime = 0.f;
	FVector PillarMeshBaseRelativeLocation = FVector::ZeroVector;
};
