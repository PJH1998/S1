// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S1HealItem.generated.h"

class UStaticMeshComponent;
class US1DissolveComponent;
class URotatingMovementComponent;

// 힐 스킬 몽타주 중 손에 부착되는 소품 — 캐스트마다 스폰/Destroy되는 순수 비주얼 액터(복제 없음, 각 머신이 로컬로 스폰)
UCLASS()
class S1_API AS1HealItem : public AActor
{
	GENERATED_BODY()

public:
	AS1HealItem();

	// 소켓 부착 후 호출 — Dissolve로 나타남
	void PlaySpawnDissolve(float Duration);
	// Dissolve로 사라진 뒤 자동으로 Destroy됨
	void PlayDespawnDissolve(float Duration);

	void SetRotationRate(const FRotator& RotationRate);

	UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1DissolveComponent> DissolveComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent;

private:
	void HandleDespawnDissolveComplete();
};
