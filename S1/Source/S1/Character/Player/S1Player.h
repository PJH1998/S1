// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "Data/S1InputData.h"
#include "S1Enums.h"
#include "S1Player.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class AS1PlayerController;
class AS1Weapon;
class US1LockOnComponent;
class US1PlayerCameraComponent;
class US1WeaponAnimLayer;

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

	void			    ActivateAbility(const FGameplayTag& AbilityTag);
	void			    ReleaseAbility(const FGameplayTag& AbilityTag);
	void			    SetSprinting(bool bInSprint);
	bool			    GetSprinting();
	AS1Weapon*		    GetEquippedWeapon()    const { return EquippedWeapon; }
	US1LockOnComponent* GetLockOnComponent()   const { return LockOnComponent; }
	const TArray<FS1AbilityInputBinding>& GetAbilityInputBindings() const { return AbilityInputBindings; }

	UFUNCTION()
	void OnItemEquipped(FGameplayTag ItemTag);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

	// 장착 무기 없을 때 사용하는 기본값 (캐릭터 BP별로 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
	TSubclassOf<AS1Weapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
	TSubclassOf<US1WeaponAnimLayer> DefaultAnimLayerClass;

	// 무기 없을 때(기본 상태) 부여할 GA 그룹 태그
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
	FGameplayTag DefaultWeaponAbilitiesTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AS1Weapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("w_R_Attach");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float WeaponLayerBlendTime = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FS1AbilityInputBinding> AbilityInputBindings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1LockOnComponent> LockOnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1PlayerCameraComponent> Camera;

private:
	// 무기 무관 기본 GA (Dash, Dodge 등) — 게임 시작 시 1회 등록
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag DefaultAbilitiesTag;

	// 현재 장착된 무기의 WeaponType — 같은 타입 재장착 시 AnimLayer 교체 건너뜀
	ES1WeaponType CurrentWeaponType = ES1WeaponType::None;

	// 현재 부여 중인 무기 GA 그룹 태그 (회수 시 사용)
	FGameplayTag CurrentWeaponAbilitiesTag;

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

	void EquipWeapon(const FGameplayTag& ItemTag);

	// Jump() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag JumpEventTag;

public:
	// GA_Action::OnMoveBeginReceived에서 S키(반대 방향) 체크용
	FVector GetLastInputDirection() const { return LastInputDirection; }
	// OnMove(Triggered/Completed)에서 설정 — 키 릴리즈 시 ZeroVector로 리셋
	void SetLastInputDirection(const FVector& InDirection) { LastInputDirection = InDirection; }

private:
	// 현재 이동 입력 방향 (키 없으면 ZeroVector, OnMove Completed에서 리셋)
	FVector LastInputDirection = FVector::ZeroVector;

	// Landed() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag LandedEventTag;
};
