// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "S1Player.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class AS1PlayerController;

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

public:
	virtual void	InitSystem() override;

	void			ActivateAbility(const FGameplayTag& AbilityTag);
	void			SetSprinting(bool bInSprint);
	bool			GetSprinting();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

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
};
