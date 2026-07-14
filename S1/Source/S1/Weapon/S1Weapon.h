// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UNiagaraComponent;

DECLARE_MULTICAST_DELEGATE(FOnS1HitCollisionEnabled)

UCLASS()
class S1_API AS1Weapon : public AActor
{
	GENERATED_BODY()

public:
	AS1Weapon();

	void EnableHitCollision(float InAtkScale = 1.0f, FGameplayTag InHitStrengthTag = FGameplayTag(), FGameplayTag InSoundBaseTag = FGameplayTag());
	void DisableHitCollision();

	float        GetCurrentAtkScale()       const { return CurrentAtkScale; }
	FGameplayTag GetCurrentHitStrengthTag() const { return CurrentHitStrengthTag; }
	FGameplayTag GetCurrentSoundBaseTag()   const { return CurrentSoundBaseTag; }

	void EnableTrail();
	void DisableTrail();

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UBoxComponent* GetAttackBox() const { return AttackBox; }
	ES1WeaponType GetWeaponType() const { return WeaponType; }

	FOnS1HitCollisionEnabled OnHitCollisionEnabled;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> AttackBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	ES1WeaponType WeaponType = ES1WeaponType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Trail")
	FName TipSocketName = TEXT("Tip");

private:
	float        CurrentAtkScale       = 1.0f;
	FGameplayTag CurrentHitStrengthTag;
	FGameplayTag CurrentSoundBaseTag;
};
