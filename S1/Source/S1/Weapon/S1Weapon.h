// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S1Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE(FOnS1HitCollisionEnabled)

UCLASS()
class S1_API AS1Weapon : public AActor
{
	GENERATED_BODY()

public:
	AS1Weapon();

	void EnableHitCollision();
	void DisableHitCollision();

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UBoxComponent* GetAttackBox() const { return AttackBox; }

	FOnS1HitCollisionEnabled OnHitCollisionEnabled;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> AttackBox;
};
