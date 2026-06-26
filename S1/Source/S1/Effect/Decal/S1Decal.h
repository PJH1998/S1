// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/S1Effect.h"
#include "Interface/S1PoolingInterface.h"
#include "S1Decal.generated.h"

class UDecalComponent;
class UMaterialInstanceDynamic;

UCLASS()
class S1_API AS1Decal : public AS1Effect, public IS1PoolingInterface
{
	GENERATED_BODY()

public:
	AS1Decal();

public:
	virtual void ShowDecal(const FVector& InLocation, const FRotator& InRotation, const FVector& InDecalSize);
	virtual void HideDecal();
	virtual bool UpdateDecal(float DeltaTime);

	bool IsActive() const { return bIsActive; }

	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation) override;
	virtual void OnReturnToPool() override;

protected:
	UMaterialInstanceDynamic* CreateAndApplyDynamicMaterial();

	// 머티리얼(셰이더맵)이 완성됐을 때만 데칼을 노출한다. 아직이면 false 반환(노출 보류).
	bool TryReveal();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	TObjectPtr<UDecalComponent> DecalComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

private:
	bool bIsActive = false;

	// 노출 대기 상태. 머티리얼 컴파일 완료 전까지 true로 두고, TryReveal에서 완료 확인 후 노출.
	bool bPendingReveal = false;

};
