// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/S1Effect.h"
#include "S1Decal.generated.h"

class UDecalComponent;
class UMaterialInstanceDynamic;

UCLASS()
class S1_API AS1Decal : public AS1Effect
{
	GENERATED_BODY()
	
public:	
	AS1Decal();

public:
	virtual void ShowDecal(const FVector& InLocation, const FRotator& InRotation, const FVector& InDecalSize);
	virtual void HideDecal();
	virtual bool UpdateDecal(float DeltaTime);

	bool IsActive() const { return bIsActive; }

protected:
	UMaterialInstanceDynamic* CreateAndApplyDynamicMaterial();

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

};
