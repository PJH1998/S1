// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S1Weapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class S1_API AS1Weapon : public AActor
{
	GENERATED_BODY()

public:
	AS1Weapon();

	void DoLineTrace(TArray<FHitResult>& OutHitResults);

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Socket")
	FName TraceStartSocketName = FName("w_TraceStart");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Socket")
	FName TraceEndSocketName = FName("w_TraceEnd");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	float TraceRadius = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
};
