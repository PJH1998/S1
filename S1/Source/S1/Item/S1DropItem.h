// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Monster/S1DropItemResource.h"
#include "GameFramework/Actor.h"
#include "Interface/S1PoolingInterface.h"
#include "S1DropItem.generated.h"

class AController;
class UMaterialInstanceDynamic;
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

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation) override;
	virtual void OnReturnToPool() override;

	void InitializeDrop(ES1DropItemType InDropType, int32 InAmount, FGameplayTag InItemTag, FGameplayTag InRarityTag, const FS1DropItemResourceEntry& Resource, AController* InOwnerController);

protected:
	void BindGoldResource(const FS1DropItemResourceEntry& Resource);
	void BindExpResource(const FS1DropItemResourceEntry& Resource);
	void BindItemResource(const FS1DropItemResourceEntry& Resource);
	void BindBaseResource(const FS1DropItemResourceEntry& Resource);

	void StartDropPresentation();
	void UpdateDropPresentation(float DeltaTime);
	FVector CalculateDropTargetLocation() const;
	bool CanPickup(AActor* OtherActor) const;
	void Pickup();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PickupRadius = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationDuration = { 1.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationDistance = { 300.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float PresentationHeight = { 300.f };

private:
	ES1DropItemType DropType = ES1DropItemType::Item;

	FGameplayTag ItemTag;
	FGameplayTag RarityTag;

	TWeakObjectPtr<AController> OwnerController;

	FVector PresentationStartLocation = FVector::ZeroVector;
	FVector PresentationTargetLocation = FVector::ZeroVector;

	int32 Amount = 0;
	float PresentationElapsedTime = 0.f;
	bool bIsPresenting = false;
};
