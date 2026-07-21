// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Monster/S1DropItemResource.h"
#include "GameFramework/Actor.h"
#include "Interface/S1PoolingInterface.h"
#include "S1DropItem.generated.h"

class FLifetimeProperty;

class AController;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

UCLASS()
class S1_API AS1DropItem : public AActor, public IS1PoolingInterface
{
	GENERATED_BODY()
	
public:	
	AS1DropItem();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation) override;
	virtual void OnReturnToPool() override;

	void InitializeDrop(ES1DropItemType InDropType, int32 InAmount, FGameplayTag InItemTag, FGameplayTag InRarityTag, const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams, AController* InOwnerController);

protected:
	void BindGoldResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams);
	void BindExpResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams);
	void BindItemResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams);
	void BindBaseResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams);
	void ApplyVisualParams(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams);

	void StartDropPresentation();
	void EnablePickupCollision();
	void UpdateDropPresentation(float DeltaTime);
	void UpdateFloatingMotion(float DeltaTime);
	FVector CalculateDropTargetLocation() const;
	bool CanPickup(AActor* OtherActor) const;
	void Pickup();
	void ApplyReplicatedDropState();

	UFUNCTION()
	void OnRep_DropState();

	// 픽업 성공을 owner 클라(+서버)에 알려 사운드를 재생시킨다. bOnlyRelevantToOwner라 다른 클라에는 전달되지 않는다.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayPickupSound();

	UFUNCTION()
	void OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> DynamicMaterialSource;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PickupRadius = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationDuration = { 1.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationDistance = { 300.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationHeight = { 300.f };

private:
	UPROPERTY(ReplicatedUsing = OnRep_DropState)
	ES1DropItemType DropType = ES1DropItemType::Item;

	UPROPERTY(ReplicatedUsing = OnRep_DropState)
	FGameplayTag ItemTag;

	UPROPERTY(ReplicatedUsing = OnRep_DropState)
	FGameplayTag RarityTag;

	UPROPERTY(Replicated)
	TObjectPtr<AController> OwnerController;

	FVector PresentationStartLocation = FVector::ZeroVector;
	FVector PresentationTargetLocation = FVector::ZeroVector;
	FVector MeshBaseRelativeLocation = FVector::ZeroVector;

	UPROPERTY(ReplicatedUsing = OnRep_DropState)
	int32 Amount = 0;

	// 서버가 InitializeDrop을 호출한 시점의 스폰 위치 — 클라이언트가 프레젠테이션 시작점 기준으로 사용
	UPROPERTY(Replicated)
	FVector ReplicatedSpawnLocation = FVector::ZeroVector;

	// 서버가 계산한 최종 정착 위치 — 클라이언트가 동일한 위치로 보간(보이는 위치 = 충돌 위치 일치).
	// 재사용 시마다 난수로 갱신되므로, 이 값을 OnRep 트리거로 삼아 풀 재사용(예: 동일 Amount의 Exp) 시에도 클라가 항상 재연출하도록 한다.
	UPROPERTY(ReplicatedUsing = OnRep_DropState)
	FVector ReplicatedRestLocation = FVector::ZeroVector;

	float PresentationElapsedTime = 0.f;
	float FloatingElapsedTime = 0.f;
	float FloatingAmplitude = 10.f;
	float FloatingSpeed = 2.5f;
	bool bIsPresenting = false;
};
