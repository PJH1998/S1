// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Boss/S1BossBase.h"
#include "S1Boss_000.generated.h"

class USkeletalMeshComponent;

// 보스가 노출하는 무기. SKM_Boss000은 검/도끼를 한 메쉬에 모두 포함하므로 한쪽만 보이게 토글한다.
UENUM(BlueprintType)
enum class EBossWeapon : uint8
{
	Axe,    // 1페이즈
	Sword,  // 2페이즈
};

/*
[BOSS] Cobalt
 */
UCLASS()
class S1_API AS1Boss_000 : public AS1BossBase
{
	GENERATED_BODY()

public:
	AS1Boss_000();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 노출 무기 전환(서버 권위). 페이즈 전환 지점(어빌리티/AI)에서 호출한다.
	void SetActiveWeapon(EBossWeapon NewWeapon);

	// Wall-Kick dash 등 절대 타깃점 이동의 목표 위치. 서버에서 어빌리티가 설정, 이동 노티가 읽음.
	void SetWallKickTarget(const FVector& InTarget) { WallKickTarget = InTarget; }
	FVector GetWallKickTarget() const { return WallKickTarget; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnRep_ActiveWeapon();

	// ActiveWeapon 값에 맞춰 무기 메쉬를 표시/숨김(서버·클라 공용).
	void UpdateWeaponVisibility();

	// 왼손(w_L_Attach)에 부착하는 방패. Leader Pose로 보스 본을 따라가며, 1페이즈(Axe)에서만 표시.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ShieldMesh;

	// 기본값 Axe(1페이즈)
	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	EBossWeapon ActiveWeapon = EBossWeapon::Axe;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Chase")
	float DefaultMaxWalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Chase")
	float DefaultMaxAcceleration = 2048.f;

	// Wall-Kick dash 목표 위치(서버 전용 transient).
	FVector WallKickTarget = FVector::ZeroVector;
};
