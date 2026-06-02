// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "S1Player.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class AS1PlayerController;
class AS1Weapon;

UCLASS()
class S1_API AS1Player : public AS1Character
{
	GENERATED_BODY()
	
public:
	AS1Player();

protected:
	virtual void	BeginPlay() override;
	virtual void	PossessedBy(AController* NewController) override;
	virtual void	Landed(const FHitResult& Hit) override;

public:
	virtual void	Tick(float DeltaTime) override;
	virtual void	Jump() override;
	virtual void	AddMovementInput(FVector WorldDirection, float ScaleValue = 1.f, bool bForce = false) override;

public:
	virtual void	InitSystem() override;

	void			ActivateAbility(const FGameplayTag& AbilityTag);
	void			SetSprinting(bool bInSprint);
	bool			GetSprinting();
	AS1Weapon*		GetEquippedWeapon() const { return EquippedWeapon; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AS1Weapon> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AS1Weapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("w_R_Attach");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

private:
	UPROPERTY(EditAnywhere, Category = "Ability System Component")
	FGameplayTag CharacterAbilitiesTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AS1PlayerController> PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 1000.f;

	bool bSprint = false;

	// 액션 중 점프 차단 조건 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag ActionStateTag;

	// 공중 상태 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag AirStateTag;

	// Jump() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag JumpEventTag;

	// Landed() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag LandedEventTag;
};
